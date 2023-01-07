#include "minicio.h"

int main() {
    int arr[10];
    int i;
    for ( i = 0; i < 10; i = i + 1 )
    {
        arr[i] = 2 * i;
    }

    for ( i = 0; i < 10; i = i + 1 ) putint(arr[i]);
    
    return 0;
}
