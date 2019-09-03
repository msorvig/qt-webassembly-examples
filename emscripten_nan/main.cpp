
#include <assert.h>
#include <limits>

int main()
{
    static_assert(std::numeric_limits<double>::has_signaling_NaN, "eep");
    return std::numeric_limits<double>::signaling_NaN();
}
