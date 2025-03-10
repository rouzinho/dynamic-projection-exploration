#include "plugin.h"
#include "RosLatentSubscriber.h"
#include <cedar/processing/ElementDeclaration.h>

void pluginDeclaration(cedar::aux::PluginDeclarationListPtr plugin)
{
    cedar::proc::ElementDeclarationPtr summation_decl
    (
        new cedar::proc::ElementDeclarationTemplate <RosLatentSubscriber>("Utilities")
    );
    plugin->add(summation_decl);
}
