#include "plugin.h"
#include "Entropy.h"
#include <cedar/processing/ElementDeclaration.h>

void pluginDeclaration(cedar::aux::PluginDeclarationListPtr plugin)
{
    cedar::proc::ElementDeclarationPtr summation_decl
    (
        new cedar::proc::ElementDeclarationTemplate <Entropy>("Utilities")
    );
    plugin->add(summation_decl);
}
