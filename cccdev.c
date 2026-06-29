#include "sys/errno.h"
#include <sys/param.h>
#include <sys/module.h>
#include <sys/kernel.h>
#include <sys/systm.h>
#include <sys/conf.h>
#include <sys/uio.h>
#include <sys/malloc.h>

MALLOC_DECLARE(M_CCCDEV);
MALLOC_DEFINE(M_CCCDEV, "malloc_cccdev", "malloc c crypto character device");

#include "session.h"


static struct cdev *sdev;

static d_open_t     d_open;
static d_close_t    d_close;
static d_read_t     d_read;
static d_write_t    d_write;
static d_ioctl_t    d_ioctl;

static struct cdevsw cccdev_cdevsw = {
    .d_version  = D_VERSION,
    .d_open     = d_open,
    .d_close    = d_close,
    .d_read     = d_read,
    .d_write    = d_write,
    .d_ioctl    = d_ioctl,
    .d_name     = "caesar",
};




static int d_open(struct cdev *dev, int oflags, int devtype, struct thread *td) {
    (void)create_new_session();
    return (0);
}

static int d_close(struct cdev *dev, int fflag, int devtype, struct thread *td) {
    return (0);
}

static int d_read(struct cdev *dev, struct uio *uio, int ioflag) {
    struct session *session;
    int error;

    session = get_session();
    if (session == NULL) return (0);
    error = session_to_uio(session, uio);

    return (error);
}

static int d_write(struct cdev *dev, struct uio *uio, int ioflag) {
    struct session *session;
    int error;

    session = get_session();
    if (session == NULL) return (0);
    error = uio_to_session(uio, session);

    return (error);
}

static int d_ioctl(struct cdev *dev, u_long cmd, caddr_t data, int fflag, struct thread *td) {
    int error = 0;

    struct session *session = get_session();
    if (session == NULL)
        return (EIO);

    switch (cmd) {
    /*
     * case *NAME*:
     *   // read data
     *   int kernel_integer = *(int *)data;
     *
     *   // set data
     *   *(int *)data = 10;
     *
     *   break;
     */
    case CCCDEV_IOCTL_RESET:
        session->settings = (struct settings){0};
        break;
    case CCCDEV_IOCTL_SET_FUNC:
        session->settings.function = *(enum crypt_function *)data;
        /* FUNC_UNDEFINED is still a valid value */
        if (
            session->settings.function <  FUNC_UNDEFINED ||
            session->settings.function >= FUNC_ENUM_END
        ) {
            error = ENOTTY;
            session->settings.function = FUNC_UNDEFINED;
        }
        break;
    case CCCDEV_IOCTL_SET_ALG:
        session->settings.algorithm = *(enum crypt_algorithm *)data;
        /* ALG_UNDEFINED is still a valid value */
        if (
            session->settings.algorithm <  ALG_UNDEFINED ||
            session->settings.algorithm >= ALG_ENUM_END
        ) {
            error = ENOTTY;
            session->settings.function = ALG_UNDEFINED;
        }
        break;
    case CCCDEV_IOCTL_SET_SETTINGS:
        session->settings.settings = *(union algorithm_settings *)data;
        break;
    case CCCDEV_IOCTL_GET_FUNC:
        *(enum crypt_function *)data = session->settings.function;
        break;
    case CCCDEV_IOCTL_GET_ALG:
        *(enum crypt_algorithm *)data = session->settings.algorithm;
        break;
    case CCCDEV_IOCTL_GET_SETTINGS:
        *(union algorithm_settings *)data = session->settings.settings;
        break;
    default:
        error = ENOTTY;
        break;
    }

    return (error);
}





static int d_modevent(module_t mod, int type, void *arg) {
    int error = 0;

    switch (type) {
    case MOD_LOAD:
        sdev = make_dev(
            &cccdev_cdevsw,
            0,
            UID_ROOT,
            GID_WHEEL,
            0666,
            "cccdev"
        );

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
