file(GLOB local_source
"*.h"
"*.hpp"
"*.cpp"
)

set(coroutine_source ${local_source} PARENT_SCOPE)
