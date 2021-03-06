#pragma once

#include <bdn/ui/Checkbox.h>

#import <bdn/mac/ViewCore.hh>
#import <bdn/mac/ui_util.hh>
#import <bdn/mac/util.hh>

namespace bdn::ui::mac
{
    class CheckboxCore : public ViewCore, virtual public Checkbox::Core
    {
      private:
        static NSButton *createNSButton();

      public:
        CheckboxCore(const std::shared_ptr<ViewCoreFactory> &viewCoreFactory);

      public:
        void handleClick();

      private:
        NSButton *_nsButton;
    };
}
