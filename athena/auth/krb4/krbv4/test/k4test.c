#include <krb.h>

void
main(
    void
    )
{
    char test[1024];
    int len = sizeof(test);
    printf("krb.conf: %s\n", krb_get_krbconf2(test, &len));
    len = sizeof(test);
    printf("krb.realms: %s\n", krb_get_krbrealm2(test, &len));
}
