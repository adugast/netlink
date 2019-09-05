#ifndef __NETLINK_H__
#define __NETLINK_H__


#ifdef __cplusplus
extern "C"
{
#endif


/*! Opaque type for the netlink handler */
typedef struct nl nl_t;


/*!
 * \brief   Signature of user netlink callback
 *
 * This callback will be triggered when a NETLINK_KOBJECT_UEVENT msg is received
 *
 * \param   msglen  OUT     size of the message received
 * \param   msg     OUT     netlink message received
 */
typedef void (*nl_reader_cb_t)(ssize_t msglen, char *msg, void *ctx);


/*!
 * \brief   Create a netlink handler
 *
 * \param   cb          IN  callback triggered when a netlink mesage is received
 * \param   buffer_size IN  size of the buffer that will received the message
 *                           - if too small, the message will be truncated.
 * \param   ctx         IN  user context
 *
 * \return  netlink handler in case of success, NULL in case of error
 */
nl_t *nl_init_handler(nl_reader_cb_t cb, ssize_t buffer_size, void *ctx);


/*!
 * \brief Launch a netlink listener over the handler
 *
 * Launch poll loop over a netlink socket that waits for events.
 * When a netlink message is received, the nl_reader_cb_t callback is triggered
 *
 * \param   hdl IN  netlink handler
 *
 * \return  -1 in case of failure, 0 in case of success
 */
int nl_launch_listener(nl_t *hdl);


/*!
 * \brief  Delete a netlink handler
 *
 * \param  hdl IN  netlink handler to free
 */
void nl_deinit_handler(nl_t *hdl);


#ifdef __cplusplus
}
#endif


#endif /* __NETLINK_H__ */

