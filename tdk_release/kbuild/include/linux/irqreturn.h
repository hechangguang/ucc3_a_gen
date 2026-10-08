/*
 * irqflags.h -- tsp 中断返回值头文件
 * Created by : Top-Silicon Software Team
 * Copyright : (C) 2023 Hangzhou Top-Silicon Microelectronics Co., Ltd. All rights reserved.
 * 
 * This file is the property of Hangzhou Top-Silicon Microelectronics Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Hangzhou Top-Silicon Microelectronics Co., Ltd.
 */

#ifndef __LINUX_IRQRETURN_H__
#define __LINUX_IRQRETURN_H__

/**
 * enum irqreturn
 * @IRQ_NONE            interrupt was not from this device
 * @IRQ_HANDLED         interrupt was handled by this device
 * @IRQ_WAKE_THREAD     handler requests to wake the handler thread
 */
enum irqreturn {
    IRQ_NONE            = (0 << 0),
    IRQ_HANDLED         = (1 << 0),
    IRQ_WAKE_THREAD     = (1 << 1),
};

typedef enum irqreturn irqreturn_t;

#define IRQ_RETVAL(x)   ((x) != IRQ_NONE)

#endif /* __LINUX_IRQRETURN_H__ */
