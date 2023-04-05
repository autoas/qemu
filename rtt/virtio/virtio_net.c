/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-11-11     GuEe-GUI     the first version
 * 2023-04-05     Parai Wang   for ssas
 */

#include <rthw.h>
#include <rtthread.h>
#include <cpuport.h>
#include <virtio_net.h>
#include "device.h"

rt_err_t eth_device_ready(struct eth_device* dev)
{
    return RT_EOK;
}

rt_err_t eth_device_init(struct eth_device * dev, const char *name)
{
    return rt_device_register((rt_device_t)dev, name, RT_DEVICE_FLAG_RDWR | RT_DEVICE_FLAG_REMOVABLE);
}

rt_err_t eth_device_linkchange(struct eth_device* dev, rt_bool_t up)
{
    dev->link_changed = 0x01;
    if (up == RT_TRUE)
        dev->link_status = 0x01;
    else
        dev->link_status = 0x00;
    return RT_EOK;
}

static rt_ssize_t virtio_net_write(rt_device_t dev, rt_off_t pos, const void *buffer, rt_size_t count)
{
    rt_uint16_t id;
    struct virtio_net_device *virtio_net_dev = (struct virtio_net_device *)dev;
    struct virtio_device *virtio_dev = &virtio_net_dev->virtio_dev;
    struct virtq *queue_tx = &virtio_dev->queues[VIRTIO_NET_QUEUE_TX];

#ifdef RT_USING_SMP
    rt_base_t level = rt_spin_lock_irqsave(&virtio_dev->spinlock);
#endif

    id = (queue_tx->avail->idx * 2) % queue_tx->num;

    virtio_net_dev->info[id].hdr.flags = 0;
    virtio_net_dev->info[id].hdr.gso_type = 0;
    virtio_net_dev->info[id].hdr.hdr_len = 0;
    virtio_net_dev->info[id].hdr.gso_size = 0;
    virtio_net_dev->info[id].hdr.csum_start = 0;
    virtio_net_dev->info[id].hdr.csum_offset = 0;
    virtio_net_dev->info[id].hdr.num_buffers = 0;

    virtio_free_desc(virtio_dev, VIRTIO_NET_QUEUE_TX, id);
    virtio_free_desc(virtio_dev, VIRTIO_NET_QUEUE_TX, id + 1);

    virtio_fill_desc(virtio_dev, VIRTIO_NET_QUEUE_TX, id,
            VIRTIO_VA2PA(&virtio_net_dev->info[id].hdr), VIRTIO_NET_HDR_SIZE, VIRTQ_DESC_F_NEXT, id + 1);

    virtio_fill_desc(virtio_dev, VIRTIO_NET_QUEUE_TX, id + 1,
            VIRTIO_VA2PA(buffer), count, 0, 0);

    virtio_submit_chain(virtio_dev, VIRTIO_NET_QUEUE_TX, id);

    virtio_queue_notify(virtio_dev, VIRTIO_NET_QUEUE_TX);

    virtio_alloc_desc(virtio_dev, VIRTIO_NET_QUEUE_TX);
    virtio_alloc_desc(virtio_dev, VIRTIO_NET_QUEUE_TX);

#ifdef RT_USING_SMP
    rt_spin_unlock_irqrestore(&virtio_dev->spinlock, level);
#endif

    return count;
}

static rt_ssize_t virtio_net_read(rt_device_t dev, rt_off_t pos, void *buffer, rt_size_t count)
{
    rt_uint16_t id;
    rt_ssize_t len = 0;
    struct virtio_net_device *virtio_net_dev = (struct virtio_net_device *)dev;
    struct virtio_device *virtio_dev = &virtio_net_dev->virtio_dev;
    struct virtq *queue_rx = &virtio_dev->queues[VIRTIO_NET_QUEUE_RX];

    if (queue_rx->used_idx != queue_rx->used->idx)
    {
#ifdef RT_USING_SMP
        rt_base_t level = rt_spin_lock_irqsave(&virtio_dev->spinlock);
#endif
        id = (queue_rx->used->ring[queue_rx->used_idx % queue_rx->num].id + 1) % queue_rx->num;
        len = queue_rx->used->ring[queue_rx->used_idx % queue_rx->num].len - VIRTIO_NET_HDR_SIZE;

#ifdef RT_USING_SMP
        rt_spin_unlock_irqrestore(&virtio_dev->spinlock, level);
#endif
        if (len > VIRTIO_NET_PAYLOAD_MAX_SIZE)
        {
            rt_kprintf("%s: Receive buffer's size = %u is too big!\n", virtio_net_dev->parent.parent.parent.name, len);
            len = VIRTIO_NET_PAYLOAD_MAX_SIZE;
        }

#ifdef RT_USING_SMP
        level = rt_spin_lock_irqsave(&virtio_dev->spinlock);
#endif
        rt_memcpy(buffer, (void *)VIRTIO_PA2VA(queue_rx->desc[id].addr), len);

        queue_rx->used_idx++;

        virtio_submit_chain(virtio_dev, VIRTIO_NET_QUEUE_RX, id - 1);

        virtio_queue_notify(virtio_dev, VIRTIO_NET_QUEUE_RX);

#ifdef RT_USING_SMP
        rt_spin_unlock_irqrestore(&virtio_dev->spinlock, level);
#endif

    }

    return len;
}

static rt_err_t virtio_net_init(rt_device_t dev)
{
    int i;
    rt_uint16_t idx[VIRTIO_NET_RTX_QUEUE_SIZE];
    struct virtio_net_device *virtio_net_dev = (struct virtio_net_device *)dev;
    struct virtio_device *virtio_dev = &virtio_net_dev->virtio_dev;
    struct virtq *queue_rx, *queue_tx;

    queue_rx = &virtio_dev->queues[VIRTIO_NET_QUEUE_RX];
    queue_tx = &virtio_dev->queues[VIRTIO_NET_QUEUE_TX];

    virtio_alloc_desc_chain(virtio_dev, VIRTIO_NET_QUEUE_RX, queue_rx->num, idx);
    virtio_alloc_desc_chain(virtio_dev, VIRTIO_NET_QUEUE_TX, queue_tx->num, idx);

    for (i = 0; i < queue_rx->num; ++i)
    {
        rt_uint16_t id = (i * 2) % queue_rx->num;
        void *addr = virtio_net_dev->info[i].tx_buffer;

        /* Descriptor for net_hdr */
        virtio_fill_desc(virtio_dev, VIRTIO_NET_QUEUE_RX, id,
                VIRTIO_VA2PA(addr), VIRTIO_NET_HDR_SIZE, VIRTQ_DESC_F_NEXT | VIRTQ_DESC_F_WRITE, id + 1);

        /* Descriptor for data */
        virtio_fill_desc(virtio_dev, VIRTIO_NET_QUEUE_RX, id + 1,
                VIRTIO_VA2PA(addr) + VIRTIO_NET_HDR_SIZE, VIRTIO_NET_MSS, VIRTQ_DESC_F_WRITE, 0);

        queue_rx->avail->ring[i] = id;
    }
    rt_hw_dsb();

    queue_rx->avail->flags = 0;
    queue_rx->avail->idx = queue_rx->num;

    queue_rx->used_idx = queue_rx->used->idx;

    queue_tx->avail->flags = VIRTQ_AVAIL_F_NO_INTERRUPT;
    queue_tx->avail->idx = 0;

    virtio_queue_notify(virtio_dev, VIRTIO_NET_QUEUE_RX);

    return eth_device_linkchange(&virtio_net_dev->parent, RT_TRUE);
}

static rt_err_t virtio_net_control(rt_device_t dev, int cmd, void *args)
{
    rt_err_t status = RT_EOK;
    struct virtio_net_device *virtio_net_dev = (struct virtio_net_device *)dev;

    switch (cmd)
    {
    case NIOCTL_GADDR:
        if (args == RT_NULL)
        {
            status = -RT_ERROR;
            break;
        }

        rt_memcpy(args, virtio_net_dev->config->mac, sizeof(virtio_net_dev->config->mac));
        break;
    default:
        status = -RT_EINVAL;
        break;
    }

    return status;
}

#ifdef RT_USING_DEVICE_OPS
const static struct rt_device_ops virtio_net_ops =
{
    virtio_net_init,
    RT_NULL,
    RT_NULL,
    virtio_net_read,
    virtio_net_write,
    virtio_net_control
};
#endif

static void virtio_net_isr(int irqno, void *param)
{
    struct virtio_net_device *virtio_net_dev = (struct virtio_net_device *)param;
    struct virtio_device *virtio_dev = &virtio_net_dev->virtio_dev;
    struct virtq *queue_rx = &virtio_dev->queues[VIRTIO_NET_QUEUE_RX];

#ifdef RT_USING_SMP
    rt_base_t level = rt_spin_lock_irqsave(&virtio_dev->spinlock);
#endif

    virtio_interrupt_ack(virtio_dev);
    rt_hw_dsb();

    if (queue_rx->used_idx != queue_rx->used->idx)
    {
        rt_hw_dsb();

        eth_device_ready(&virtio_net_dev->parent);
    }

#ifdef RT_USING_SMP
    rt_spin_unlock_irqrestore(&virtio_dev->spinlock, level);
#endif
}

rt_err_t rt_virtio_net_init(rt_ubase_t *mmio_base, rt_uint32_t irq)
{
    static int dev_no = 0;
    char dev_name[RT_NAME_MAX];
    struct virtio_device *virtio_dev;
    struct virtio_net_device *virtio_net_dev;

    virtio_net_dev = rt_malloc(sizeof(struct virtio_net_device));

    if (virtio_net_dev == RT_NULL)
    {
        goto _alloc_fail;
    }

    virtio_dev = &virtio_net_dev->virtio_dev;
    virtio_dev->irq = irq;
    virtio_dev->mmio_base = mmio_base;

    virtio_net_dev->config = (struct virtio_net_config *)virtio_dev->mmio_config->config;

#ifdef RT_USING_SMP
    rt_spin_lock_init(&virtio_dev->spinlock);
#endif

    virtio_reset_device(virtio_dev);
    virtio_status_acknowledge_driver(virtio_dev);

    virtio_dev->mmio_config->driver_features = virtio_dev->mmio_config->device_features & ~(
            (1 << VIRTIO_NET_F_CTRL_VQ) |
            (1 << VIRTIO_F_RING_EVENT_IDX));

    virtio_status_driver_ok(virtio_dev);

    if (virtio_queues_alloc(virtio_dev, 2) != RT_EOK)
    {
        goto _alloc_fail;
    }

    if (virtio_queue_init(virtio_dev, VIRTIO_NET_QUEUE_RX, VIRTIO_NET_RTX_QUEUE_SIZE) != RT_EOK)
    {
        goto _alloc_fail;
    }

    if (virtio_queue_init(virtio_dev, VIRTIO_NET_QUEUE_TX, VIRTIO_NET_RTX_QUEUE_SIZE) != RT_EOK)
    {
        virtio_queue_destroy(virtio_dev, VIRTIO_NET_QUEUE_RX);
        goto _alloc_fail;
    }

    virtio_net_dev->parent.parent.type = RT_Device_Class_NetIf;
#ifdef RT_USING_DEVICE_OPS
    virtio_net_dev->parent.parent.ops  = &virtio_net_ops;
#else
    virtio_net_dev->parent.parent.init      = virtio_net_init;
    virtio_net_dev->parent.parent.open      = RT_NULL;
    virtio_net_dev->parent.parent.close     = RT_NULL;
    virtio_net_dev->parent.parent.read      = virtio_net_read;
    virtio_net_dev->parent.parent.write     = virtio_net_write;
    virtio_net_dev->parent.parent.control   = virtio_net_control;
#endif

    rt_snprintf(dev_name, RT_NAME_MAX, "virtio-net%d", dev_no++);

    rt_hw_interrupt_install(irq, virtio_net_isr, virtio_net_dev, dev_name);
    rt_hw_interrupt_umask(irq);

    return eth_device_init(&virtio_net_dev->parent, dev_name);

_alloc_fail:

    if (virtio_net_dev != RT_NULL)
    {
        virtio_queues_free(virtio_dev);
        rt_free(virtio_net_dev);
    }
    return -RT_ENOMEM;
}