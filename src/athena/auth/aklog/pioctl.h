/*
 * Copyright 1997 by the Massachusetts Institute of Technology
 * For distribution and copying rights, see the file "mit-copyright.h"
 *
 * History:
 *   03-03-97  dalmeida  created.
 */

#define VIOCWHEREIS		8	/* fs whereis */
#define VIOC_FILE_CELL_NAME	1	/* fs whichcell */
#define VIOC_AFS_STAT_MT_PT	9	/* fs lsmount */


struct ViceIoctl {
    char *in, *out;   /* Data to be transferred in, or
                            out */
    short in_size;     /* Size of input buffer <= 2K */
    short out_size;    /* Maximum size of output  buffer,
                          <= 2K */
};

int
pioctl(char *path,
       int id,       /* _VICEIOCTL(id) */
       struct ViceIoctl *vi_struct, /* (caddr_t) &vi_struct */
       int follow);
