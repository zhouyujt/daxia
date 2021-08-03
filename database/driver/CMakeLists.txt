file(GLOB local_source
"*.h"
"*.cpp"
"*.hpp"
)

set(driver_source ${local_source} PARENT_SCOPE)
add_subdirectory(sqlite3)






