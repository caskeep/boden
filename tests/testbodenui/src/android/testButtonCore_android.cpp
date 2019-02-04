
#include <bdn/test.h>

#include <bdn/Button.h>
#include <bdn/Window.h>
#include <bdn/test/TestButtonCore.h>
#include <bdn/android/JButton.h>
#include "TestAndroidViewCoreMixin.h"

using namespace bdn;

class TestAndroidButtonCore : public bdn::test::TestAndroidViewCoreMixin<bdn::test::TestButtonCore>
{
  public:
    TestAndroidButtonCore() : _jButton(nullptr) {}

  protected:
    void initCore() override
    {
        bdn::test::TestAndroidViewCoreMixin<bdn::test::TestButtonCore>::initCore();

        _jButton = bdn::android::JButton(_jView.getRef_());
    }

    void verifyCoreLabel() override
    {
        String expectedLabel = _button->label;

        String label = _jButton.getText();

        REQUIRE(label == expectedLabel);
    }

  protected:
    bdn::android::JButton _jButton;
};

TEST_CASE("android.ButtonCore")
{
    std::shared_ptr<TestAndroidButtonCore> test = std::make_shared<TestAndroidButtonCore>();

    test->runTests();
}
