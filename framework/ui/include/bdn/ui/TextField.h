#pragma once

#include <bdn/Notifier.h>
#include <bdn/ui/SubmitEvent.h>
#include <bdn/ui/View.h>
#include <bdn/ui/ViewUtilities.h>

namespace bdn::ui
{
    namespace detail
    {
        VIEW_CORE_REGISTRY_DECLARATION(TextField)
    }

    class TextField : public View
    {
      public:
        /** The text field's text */
        Property<String> text;

      public:
        TextField(std::shared_ptr<ViewCoreFactory> viewCoreFactory = nullptr);

        /** Informs observers of the onSubmit() notifier about a submit event.
         */
        void submit();

        /** A notifier for submit events. Subscribe to this notifier if you want
           to be notified about submit events posted to the text field. Submit
           events are posted when the user presses the Enter key or when
           submit() is called programmatically.
        */
        Notifier<const SubmitEvent &> &onSubmit();

      protected:
        void bindViewCore() override;

      private:
        Notifier<const SubmitEvent &> _onSubmit;
        WeakCallback<void()>::Receiver _submitCallbackReceiver;

      public:
        class Core
        {
          public:
            Property<String> text;

          public:
            WeakCallback<void()> submitCallback;
        };
    };
}
