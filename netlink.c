//
// Created by scphamster on 03.12.23.
//

#include <linux/netlink.h>
#include <net/genetlink.h>
#include <net/netlink.h>

#include <net/sock.h>
#include "convenient.h"

// METADATA
#define NETLINK_PROTOCOL_NUM 31
#define NLSPACE              1024

static struct sock        *nlSock    = NULL;
static struct task_struct *nl_thread = NULL;

DECLARE_WAIT_QUEUE_HEAD(gWaitQ);
static struct sk_buff *gCurrentSKB = NULL;

static void
netlink_input_handler(struct sk_buff *skb)
{
    gCurrentSKB = skb;
    int wake_result = wake_up_process(nl_thread);

    pr_info("Wake up nl_thread success? :: %s", wake_result == true ? "yes" : "no");
}

static void
netlink_recv_and_reply(struct sk_buff *skb)
{
    struct nlmsghdr *nlh;
    struct sk_buff  *skb_tx;
    char            *reply = "Reply from kernel netlink";
    int              pid, msgsz, stat;

    /* Find that this code runs in process context, the process
     * (or thread) being the one that issued the sendmsg(2) */
    PRINT_CTX();

    nlh = (struct nlmsghdr *)skb->data;
    pid = nlh->nlmsg_pid; /*pid of sending process */
    pr_info("received from PID %d:\n"
            "\"%s\"\n",
            pid,
            (char *)NLMSG_DATA(nlh));

    //--- Lets be polite and reply
    msgsz  = strnlen(reply, NLSPACE);
    skb_tx = nlmsg_new(msgsz, 0);
    if (!skb_tx) {
        pr_warn("skb alloc failed!\n");
        return;
    }

    // Setup the payload
    nlh = nlmsg_put(skb_tx, 0, 0, NLMSG_DONE, msgsz, 0);
    //    NETLINK_CB(skb_tx).dst_group = 0; /* unicast only (cb is the
    //                                       * skb's control buffer), dest group 0 => unicast */
    strncpy(nlmsg_data(nlh), reply, msgsz);

    // Send it
    stat = nlmsg_unicast(nlSock, skb_tx, pid);
    if (stat < 0)
        pr_warn("nlmsg_unicast() failed (err=%d)\n", stat);
    pr_info("reply sent\n");
}

int
netlink_handler(void *arg)
{
    struct sk_buff *skb = NULL;

    while (!kthread_should_stop()) {
        wait_event(gWaitQ, gCurrentSKB != NULL);
        pr_info("MILESTONE 0");

        skb = gCurrentSKB;
        if (skb == NULL) {
            continue;
        }

        pr_info("MILESTONE 1");
        netlink_recv_and_reply(skb);

        gCurrentSKB = NULL;
        pr_info("MILESTONE 2");
    }

    return 0;
}

static struct netlink_kernel_cfg nl_kernel_cfg = {
    .input = netlink_input_handler,
};

int
netlink_simple_intf_init(void)
{
    nlSock = netlink_kernel_create(&init_net, NETLINK_PROTOCOL_NUM, &nl_kernel_cfg);
    if (!nlSock) {
        pr_warn("netlink_kernel_create failed\n");
        return PTR_ERR(nlSock);
    }

    nl_thread = kthread_create(netlink_handler, nlSock, "netlink handler");

    return 0;


    genlmsg_new()
}

void
netlink_simple_intf_deinit(void)
{
    pr_info("netlink deinit");

    kthread_stop(nl_thread);
    netlink_kernel_release(nlSock);
}