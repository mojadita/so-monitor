/* tty.h --- definitions for tty control module.
 * Author: Luis Colorado <luiscoloradourcola@gmail.com>
 * Date: Tue Jul 30 19:57:33 EEST 2019
 * Copyright: (C) 2019 LUIS COLORADO.  All rights reserved.
 * License: BSD.
 */
#ifndef _TTY_H
#define _TTY_H

#include "mon.h"

int init_tty(struct file_info *fi);
int reset_tty(struct file_info *fi);

#endif /* _TTY_H */
