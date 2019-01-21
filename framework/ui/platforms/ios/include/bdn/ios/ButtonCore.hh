#pragma once

#include <bdn/IButtonCore.h>
#include <bdn/ClickEvent.h>
#include <bdn/Button.h>

#import <bdn/ios/ViewCore.hh>

namespace bdn
{
    namespace ios
    {

        class ButtonCore : public ViewCore, virtual public IButtonCore
        {
          private:
            static UIButton *_createUIButton(std::shared_ptr<Button> outerButton);

          public:
            ButtonCore(std::shared_ptr<Button> outerButton);
            ~ButtonCore();

            UIButton *getUIButton();
            void setLabel(const String &label) override;
            void _clicked();

          protected:
            double getFontSize() const override;

          private:
            UIButton *_button;

            NSObject *_clickManager;
        };
    }
}
