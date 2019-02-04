#pragma once

#include <bdn/android/JTextView.h>
#include <bdn/java/JString.h>

namespace bdn
{
    namespace android
    {
        constexpr const char kCompoundButtonClassName[] = "android/widget/CompoundButton";

        template <const char *javaClassName = kCompoundButtonClassName, class... ConstructorArguments>
        class JBaseCompoundButton : public JBaseTextView<javaClassName, ConstructorArguments...>
        {
          public:
            using JBaseTextView<javaClassName>::JBaseTextView;

          public:
            Method<bool()> isChecked{this, "isChecked"};
        };

        using JCompountButton = JBaseCompoundButton<>;
    }
}
