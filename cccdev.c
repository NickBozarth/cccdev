#include <sys/param.h>
#include <sys/module.h>
#include <sys/kernel.h>
#include <sys/systm.h>
#include <sys/conf.h>
#include <sys/uio.h>
// #include <sys/malloc.h>



static struct cdev *sdev;

static d_open_t     d_open;
static d_close_t    d_close;
static d_read_t     d_read;
static d_write_t    d_write;

static struct cdevsw cccdev_cdevsw = {
    .d_version  = D_VERSION,
    .d_open     = d_open,
    .d_close    = d_close,
    .d_read     = d_read,
    .d_write    = d_write,
    .d_name     = "caesar",
};




static int d_open(struct cdev *dev, int oflags, int devtype, struct thread *td) {
    return (0);
}

static int d_close(struct cdev *dev, int fflag, int devtype, struct thread *td) {
    return (0);
}

static int d_read(struct cdev *dev, struct uio *uio, int ioflag) {
    return (0);
}

static int d_write(struct cdev *dev, struct uio *uio, int ioflag) {
    return (0);
}





static int d_modevent(module_t mod, int type, void *arg) {
    int error = 0;

    switch (type) {
    case MOD_LOAD:
        sdev = make_dev(&cccdev_cdevsw, 0, UID_ROOT, GID_WHEEL, 0666, "cccdev");
        uprintf("cccdev driver loaded into kernel\n");
        break;
    case MOD_UNLOAD:
        destroy_dev(sdev);
        uprintf("cccdev driver unloaded successfully\n");
        break;
    default:
        error = EOPNOTSUPP;
        break;
    }

    return error;
}


DEV_MODULE(cccdev, d_modevent, NULL);
