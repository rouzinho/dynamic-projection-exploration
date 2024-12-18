#include "plugin.h"
#include "Preshape.h"
#include <cedar/processing/ElementDeclaration.h>

void pluginDeclaration(cedar::aux::PluginDeclarationListPtr plugin)
{
    cedar::proc::ElementDeclarationPtr summation_decl
    (
        new cedar::proc::ElementDeclarationTemplate <Preshape>("Utilities")
    );
    plugin->add(summation_decl);
}
