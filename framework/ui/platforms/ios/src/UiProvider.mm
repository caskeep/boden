

#import <UIKit/UIKit.h>
#import <bdn/ios/UiProvider.hh>
#import <bdn/ios/WindowCore.hh>
#import <bdn/ios/ContainerViewCore.hh>
#import <bdn/ios/ButtonCore.hh>
#import <bdn/ios/CheckboxCore.hh>
#import <bdn/ios/SwitchCore.hh>
#import <bdn/ios/TextViewCore.hh>
#import <bdn/ios/TextFieldCore.hh>
#import <bdn/ios/ScrollViewCore.hh>

#include <bdn/ViewCoreTypeNotSupportedError.h>

// TODO: Well, this obviously doesn't belong here. Get rid of it ASAP.
#include <iostream>

namespace bdn
{

    std::shared_ptr<IUiProvider> getDefaultUiProvider() { return bdn::ios::UiProvider::get(); }
}

namespace bdn
{
    namespace ios
    {

        std::shared_ptr<UiProvider> UiProvider::get()
        {
            static std::shared_ptr<UiProvider> globalUIProvider = std::make_shared<UiProvider>();
            return globalUIProvider;
        };

        UiProvider::UiProvider()
        {
            // iOS uses DIPs for font sizes (although they call it "points").
            // So no conversion necessary

            _semDips = UIFont.systemFontSize;

            _layoutCoordinator = std::make_shared<LayoutCoordinator>();
        }

        String UiProvider::getName() const { return "ios"; }

        std::shared_ptr<IViewCore> UiProvider::createViewCore(const String &coreTypeName, std::shared_ptr<View> view)
        {
            if (coreTypeName == ContainerView::getContainerViewCoreTypeName())
                return std::make_shared<ContainerViewCore>(std::dynamic_pointer_cast<ContainerView>(view));

            else if (coreTypeName == Button::getButtonCoreTypeName())
                return std::make_shared<ButtonCore>(std::dynamic_pointer_cast<Button>(view));

            else if (coreTypeName == Checkbox::getCheckboxCoreTypeName())
                return std::make_shared<CheckboxCore>(std::dynamic_pointer_cast<Checkbox>(view));

            else if (coreTypeName == Toggle::getToggleCoreTypeName())
                return std::make_shared<SwitchCore<Toggle>>(std::dynamic_pointer_cast<Toggle>(view));

            else if (coreTypeName == Switch::getSwitchCoreTypeName())
                return std::make_shared<SwitchCore<Switch>>(std::dynamic_pointer_cast<Switch>(view));

            else if (coreTypeName == TextView::getTextViewCoreTypeName())
                return std::make_shared<TextViewCore>(std::dynamic_pointer_cast<TextView>(view));

            else if (coreTypeName == TextField::getTextFieldCoreTypeName())
                return std::make_shared<TextFieldCore>(std::dynamic_pointer_cast<TextField>(view));

            else if (coreTypeName == ScrollView::getScrollViewCoreTypeName())
                return std::make_shared<ScrollViewCore>(std::dynamic_pointer_cast<ScrollView>(view));

            else if (coreTypeName == Window::getWindowCoreTypeName())
                return std::make_shared<WindowCore>(std::dynamic_pointer_cast<Window>(view));

            else
                throw ViewCoreTypeNotSupportedError(coreTypeName);
        }
    }
}
