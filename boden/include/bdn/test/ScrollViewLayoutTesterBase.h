#ifndef BDN_TEST_ScrollViewLayoutTesterBase_H_
#define BDN_TEST_ScrollViewLayoutTesterBase_H_

#include <bdn/Button.h>
#include <bdn/TextView.h>
#include <bdn/Dip.h>


namespace bdn
{
namespace test
{



/** A base class for classes that test scroll view preferred size calculation and layout.
    Derive the actual test class from this and then call doPreferredSizeAndLayoutTests(). You will need to provide
    an implementation for the unimplemented virtual functions in the derived class.
    
    The template parameter BaseClass indicates the class of ScrollViewLayoutTesterBase. In many cases
    one can specify bdn::Base here, to use a default base without any special functionality.
    
    Specifying a different base class can be useful if ScrollViewLayoutTesterBase is to be used
    as a kind of mixin to add functionality on top of another test class.
    
    */
template<class BaseClass>
class ScrollViewLayoutTesterBase : public BaseClass
{
public:
    ScrollViewLayoutTesterBase()
    {        
    }

    bool _horzScrollingEnabled = false;
    bool _vertScrollingEnabled = true;


    /** Returns the scroll view to use for the tests.*/
    virtual P<ScrollView> getScrollView()=0;


    /** Returns the width of the vertical scroll bar in DIPs.
    
        If the vertical scroll bar is only shown on demand during scrolling
        as an overlay (i.e. if no space is allocated for it) then this should return 0.
    */
    virtual double getVertBarWidth()=0;


    /** Returns the height of the horizontal scroll bar in DIPs.
    
        If the horizontal scroll bar is only shown on demand during scrolling
        as an overlay (i.e. if no space is allocated for it) then this should return 0.
    */
    virtual double getHorzBarHeight()=0;
                

    /** Calls calcPreferredSize on the object that is tested and returns the result. */
    virtual Size callCalcPreferredSize( const Size& availableSpace = Size::none() )=0;   

    
    /** Calculates the scrollview layout for the specified viewport size.
        This enables the layout verify functions 
        (e.g. verifyHorzBarVisible() etc.) to be used afterwards.*/
    virtual void calcLayout(const Size& viewPortSize)=0;


    /** Called after calcLayout() to verify the horizontal visibility of the horizontal
        scrollbar.

        Should cause a test fail if the visibility is not correct.        
        */
    virtual void verifyHorzBarVisible( bool expectedVisible)=0;


    /** Called after calcLayout() to verify the horizontal visibility of the vertical
        scrollbar.
        
        Should cause a test fail if the visibility is not correct.
        */
    virtual void verifyVertBarVisible( bool expectedVisible)=0;


     /** Called after calcLayout() to verify the bounding rectangle of the content view.
        
        expectedBounds contains the expected value, assuming  that the origin of the
        scroll view's content coordinate system is (0,0). If that is not the case then
        the implementation of verifyContentViewBounds must compensate accordingly.

        The verification function should ignore insignificant differences. It can
        use bdn::Dip for that.
        
        Should cause a test fail if the content bounds are not correct.

        If the content view is null then the function can be implemented in one of these two ways:
        1) It can do nothing and ignore the call.
        2) It may also compare the specified expected values against the values a content view WOULD HAVE
        if one were there, if that is possible.
        */
    virtual void verifyContentViewBounds( const Rect& expectedBounds, double maxDeviation=0)=0;


    /** Called after calcLayout() to verify the size of the scrollable area (including
        the scroll view padding and content view margins).
        
        Should cause a test fail if the visibility is not correct.
        */
    virtual void verifyScrolledAreaSize( const Size& expectedSize)=0;


    /** Called after calcLayout() to verify the size of the view port after layout.
        If scroll bars are shown then they are not part of the view port.
        
        Should cause a test fail if the view port size is not correct.
        */
    virtual void verifyViewPortSize( const Size& expectedSize)=0;                                


    /** Returns the size of 1 pixel in DIPs.
        The default implementation infers this automatically by calling View::adjustBounds.*/
    virtual Size getPixelSize()
    {
        P<View> pView = getScrollView();

        Rect baseBounds( 0, 0, 100, 100);

        // adjust the base bounds up to the nearest pixel
        baseBounds = pView->adjustBounds( baseBounds, RoundType::up, RoundType::up );               

        // then add a tiny bit of size and round up again
        Rect tinyBitBiggerBounds = Rect(baseBounds.getPosition(), baseBounds.getSize() + Size(0.001, 0.001) );
        tinyBitBiggerBounds = pView->adjustBounds( tinyBitBiggerBounds, RoundType::up, RoundType::up );               
        
        // the difference in size should be the size of 1 pixel.
        Size pixelSize = tinyBitBiggerBounds.getSize() - baseBounds.getSize();

        // sanity check: the pixel size must not be zero
        REQUIRE( pixelSize > Size(0,0) );

        return pixelSize;
    }


    /** Runs the tests. This calls testPreferredSize() and testLayout().*/
    virtual void doPreferredSizeAndLayoutTests()
    {
        SECTION("scrollable in both directions")
            doPreferredSizeAndLayoutTests(true, true);
        SECTION("only horz scrolling")
            doPreferredSizeAndLayoutTests(true, false);
        SECTION("only vert scrolling")
            doPreferredSizeAndLayoutTests(false, true);
        SECTION("no scrolling")
            doPreferredSizeAndLayoutTests(false, false);
    }


    
    virtual void doPreferredSizeAndLayoutTests( bool horzScrollingEnabled, bool vertScrollingEnabled )
    {
        _horzScrollingEnabled = horzScrollingEnabled;
        _vertScrollingEnabled = vertScrollingEnabled;

        getScrollView()->horizontalScrollingEnabled() = horzScrollingEnabled;
        getScrollView()->verticalScrollingEnabled() = vertScrollingEnabled;

        P<ScrollViewLayoutTesterBase> pThis = this;

        CONTINUE_SECTION_WHEN_IDLE(pThis)
        {
            SECTION("preferredSize")
                pThis->testPreferredSize();

            SECTION("layout")
                pThis->testLayout();
        };
    }

    

    virtual void testPreferredSize()
    {
        P<ScrollViewLayoutTesterBase> pThis = this;

        Size pixelSize = getPixelSize();

        SECTION("contentview null")
        {
            SECTION("no padding")
            {
                Size prefSize = callCalcPreferredSize( Size::none() );
                REQUIRE( prefSize == Size(0,0) );            
            }

            SECTION("with padding")
            {
                getScrollView()->padding() = UiMargin(5,6,7,8);

                Size prefSize = callCalcPreferredSize( Size::none() );
                REQUIRE( prefSize == Size(6+8, 5+7) );
            }

            SECTION("with padding and scrollview margin")
            {
                // the scrollview's own margin should not matter - only the margin of the content view
                getScrollView()->margin() = UiMargin(1,2,3,4);
                getScrollView()->padding() = UiMargin(5,6,7,8);

                Size prefSize = callCalcPreferredSize( Size::none() );
                REQUIRE( prefSize == Size(6+8, 5+7) );
            }
        }

        SECTION("contentview not null")
        {
            P<Button> pButton = newObj<Button>();
            getScrollView()->setContentView(pButton);

            pButton->margin() = UiMargin(1,2,3,4);
            getScrollView()->padding() = UiMargin(5,6,7,8);

            CONTINUE_SECTION_WHEN_IDLE( pThis, pButton, pixelSize )
            {
                double horzBarHeight = pThis->getHorzBarHeight();
                double vertBarWidth = pThis->getVertBarWidth();

                Size optimalButtonSize = pButton->calcPreferredSize();

                Size optimalSize = Size(2+4+6+8, 1+3+5+7) + optimalButtonSize;

                SECTION("unlimited space")
                {
                    // should request as much space as the content needs by default
        
                    Size prefSize = pThis->callCalcPreferredSize( Size::none() );
                    REQUIRE( prefSize == optimalSize );
                }

                SECTION("more than enough space")
                {
                    Size prefSize = pThis->callCalcPreferredSize( optimalSize + Size(100,100) );
                    REQUIRE( prefSize == optimalSize );
                }

                SECTION("exactly enough space")
                {
                    Size prefSize = pThis->callCalcPreferredSize( optimalSize );
                    REQUIRE( prefSize == optimalSize );
                }

                SECTION("less width than needed, more than enough height for scrollbar")
                {
                    Size prefSize = pThis->callCalcPreferredSize( optimalSize + Size(-pixelSize.width, 1000) );

                    if(pThis->_horzScrollingEnabled)
                    {
                        // preferred width should be the available width. preferred height should include
                        // the scrollbar
                        REQUIRE( prefSize == optimalSize + Size(-pixelSize.width,  horzBarHeight) );
                    }
                    else
                    {
                        // no horz scrolling => no additional scrollbar at bottom.
                        // Also, the content cannot shrink down to the available space so
                        // the returned width should exceed it and be the optimal width
                        REQUIRE( prefSize == optimalSize + Size(0, 0) );
                    }
                }

                SECTION("less width than needed, enough height for scrollbar")
                {
                    Size prefSize = pThis->callCalcPreferredSize( optimalSize + Size(-pixelSize.width, horzBarHeight) );

                    if(pThis->_horzScrollingEnabled)
                    {
                        // preferred width should be the available width. preferred height should include
                        // the scrollbar
                        REQUIRE( prefSize == optimalSize + Size(-pixelSize.width, horzBarHeight) );
                    }
                    else
                    {
                        // no horz scrolling => no additional scrollbar at bottom.
                        // Also, the content cannot shrink down to the available space so
                        // the returned width should exceed it and be the optimal width
                        REQUIRE( prefSize == optimalSize + Size(0, 0) );
                    }
                }

                SECTION("less width than needed, not enough height for scrollbar")
                {
                    Size prefSize = pThis->callCalcPreferredSize( optimalSize + Size(-pixelSize.width, horzBarHeight-pixelSize.height) );

                    if(pThis->_horzScrollingEnabled && pThis->_vertScrollingEnabled)
                    {
                        // this should cause both scrollbars to be shown. We should use all the available space
                        REQUIRE( prefSize == optimalSize + Size(-pixelSize.width, horzBarHeight-pixelSize.height) );
                    }
                    else if(pThis->_horzScrollingEnabled)
                    {
                        // no vertical scrolling. We will get a horizontal scrollbar, but no vertical scrollbar.
                        // Note that the height will exceed the available height, since the content view also reports
                        // a preferred size that exceeds the available height.
                        REQUIRE( prefSize == optimalSize + Size(-pixelSize.width, horzBarHeight) );
                    }
                    else
                    {
                        // no horz scrolling => no additional scrollbar at bottom
                        // Also, the content cannot shrink down to the available space so
                        // the returned width should exceed it and be the optimal width
                        REQUIRE( prefSize == optimalSize + Size(0, 0) );
                    }                    
                }

                SECTION("less height than needed, more than enough width for scrollbar")
                {
                    Size prefSize = pThis->callCalcPreferredSize(optimalSize + Size(1000, -pixelSize.height) );

                    if(pThis->_vertScrollingEnabled)
                    {
                        REQUIRE( prefSize == optimalSize + Size( vertBarWidth, -pixelSize.height) );
                    }
                    else
                    {
                        // no vert scrolling => no scrollbar added. Note that the reported preferred
                        // height should exceed the available space since the content view cannot shrink
                        // further.
                        REQUIRE( prefSize == optimalSize + Size(0, 0) );
                    }                    
                }


                SECTION("less height than needed, enough width for scrollbar")
                {
                    Size prefSize = pThis->callCalcPreferredSize( optimalSize + Size(vertBarWidth, -pixelSize.height) );

                    if(pThis->_vertScrollingEnabled)
                    {
                        REQUIRE( prefSize == optimalSize + Size(vertBarWidth, -pixelSize.height) );
                    }
                    else
                    {
                        // no vert scrolling => no scrollbar added. Note that the reported preferred
                        // height should exceed the available space since the content view cannot shrink
                        // further.
                        REQUIRE( prefSize == optimalSize + Size(0, 0) );
                    }
                }

                SECTION("less height than needed, not enough width for scrollbar")
                {
                    Size prefSize = pThis->callCalcPreferredSize( optimalSize + Size( vertBarWidth-pixelSize.width, -pixelSize.height) );

                    if( pThis->_vertScrollingEnabled && pThis->_horzScrollingEnabled)
                    {
                        // we should fill the available space
                        REQUIRE( prefSize == optimalSize + Size( vertBarWidth-pixelSize.width , -pixelSize.height) );
                    }
                    else if(pThis->_vertScrollingEnabled)
                    {                        
                        // the width should exceed the available space, since the content cannot be shrunk
                        // down further. The available height should not be exceeded, since we can scroll
                        REQUIRE( prefSize == optimalSize + Size(vertBarWidth, -pixelSize.height) );
                    }
                    else
                    {
                        // no vert scrolling => no scrollbar added. Note that the reported preferred
                        // height should exceed the available space since the content view cannot shrink
                        // further.
                        REQUIRE( prefSize == optimalSize + Size(0, 0) );
                    }
                    
                }

                SECTION("less width and less height than needed")
                {
                    Size prefSize = pThis->callCalcPreferredSize( optimalSize + Size(-pixelSize.width, -pixelSize.height) );

                    if(pThis->_vertScrollingEnabled && pThis->_horzScrollingEnabled)
                    {
                        // scroll view should simply use the available space
                        REQUIRE( prefSize == optimalSize + Size(-pixelSize.width, -pixelSize.height) );                    
                    }
                    else if(pThis->_vertScrollingEnabled)
                    {
                        // preferred size should be full width plus the size of the scrollbar (since the content
                        // view cannot shrink below that). Height should be but available height
                        REQUIRE( prefSize == optimalSize + Size(vertBarWidth, -pixelSize.height) );                    
                    }
                    else if(pThis->_horzScrollingEnabled)
                    {
                        // preferred size should be full height plus the size of the scrollbar (since the content
                        // view cannot shrink below that). Width should be but available width
                        REQUIRE( prefSize == optimalSize + Size(-pixelSize.width, horzBarHeight) );                    
                    }
                    else
                    {
                        // no scrolling. Should simply be the optimal size, since the content view cannot
                        // shrink beyond that.
                        REQUIRE( prefSize == optimalSize);
                    }
                }
            };
        }

        SECTION("contentview calcPreferredSize usage")
        {
            P<ScrollViewLayoutHelperTestContentView<TextView> > pContentView = newObj<ScrollViewLayoutHelperTestContentView<TextView> >();
            getScrollView()->setContentView(pContentView);

            // we want a content view whose width and height depend on each other. So we use a text view with
            // multiline text.
            pContentView->text() = "Lorem ipsum dolor sit amet, consectetur adipiscing elit.\nPraesent ultrices, nisi quis posuere viverra, arcu erat auctor tellus, sit amet tincidunt magna leo id velit.";

            CONTINUE_SECTION_WHEN_IDLE( pThis, pContentView, pixelSize )
            {
                Size optimalSize = pThis->callCalcPreferredSize();

                int  initialCalcCount = pContentView->getCalcPreferredSizeCallCount();

                double horzBarHeight = pThis->getHorzBarHeight();

                SECTION("available space unlimited")
                {
                    Size prefSize = pThis->callCalcPreferredSize();
                    REQUIRE( prefSize==optimalSize );
                    REQUIRE( pContentView->getCalcPreferredSizeCallCount() == initialCalcCount+1);
                }

                SECTION("space bigger or equal to needed size")
                {
                    Size prefSize = pThis->callCalcPreferredSize(  optimalSize);
                    REQUIRE( prefSize==optimalSize );
                    REQUIRE( pContentView->getCalcPreferredSizeCallCount() == initialCalcCount+1);
                }

                SECTION("width less than needed")
                {
                    Size optimalContentSize = pContentView->calcPreferredSize();
                    
                    Size prefSize = pThis->callCalcPreferredSize( optimalSize + Size(-pixelSize.width, 1000) );

                    // content view should have been asked for its preferred sizeat least once
                    REQUIRE( pContentView->getCalcPreferredSizeCallCount() > initialCalcCount  );

                    if(pThis->_horzScrollingEnabled)
                    {
                        // space for the scrollbar should have been added at bottom. Width is the available space.
                        REQUIRE( prefSize == optimalSize+Size(-pixelSize.width, horzBarHeight) );
                    }
                    else
                    {                                            
                        Size contentAvailSpace = pContentView->getLastCalcPreferredSizeAvailableSpace();

                        // available space should have been communicated to the content view
                        REQUIRE( contentAvailSpace.width < optimalContentSize.width);
                        if(pThis->_vertScrollingEnabled)
                            REQUIRE( !std::isfinite(contentAvailSpace.height) );
                        else
                            REQUIRE( contentAvailSpace.height == optimalContentSize.height+1000);

                        // the preferred width should be less than optimal. The height should have increased
                        // (since we need more lines for the text).
                        REQUIRE( prefSize.width < optimalSize.width);
                        REQUIRE( prefSize.height > optimalSize.height);
                    }
                }

                SECTION("height less than needed")
                {
                    Size optimalContentSize = pContentView->calcPreferredSize();

                    Size prefSize = pThis->callCalcPreferredSize( optimalSize + Size(1000, -pixelSize.height) );

                    // content view should have been asked for its preferred sizeat least once
                    REQUIRE( pContentView->getCalcPreferredSizeCallCount() > initialCalcCount  );

                    if(pThis->_vertScrollingEnabled)
                    {
                        // space for the scrollbar should have been added at the right side. Height is the available space.
                        REQUIRE( prefSize == optimalSize+Size(pThis->getVertBarWidth(), -pixelSize.height) );
                    }
                    else
                    {                    
                        Size contentAvailSpace = pContentView->getLastCalcPreferredSizeAvailableSpace();

                        REQUIRE( contentAvailSpace.height < optimalContentSize.height);

                        // available space should have been communicated to the content view
                        if(pThis->_horzScrollingEnabled)
                            REQUIRE( !std::isfinite(contentAvailSpace.width) );
                        else
                            REQUIRE( contentAvailSpace.width == optimalContentSize.width+1000);
                        
                        // should have reported the optimal size since text views cannot reduce their height.
                        REQUIRE( prefSize == optimalSize );
                    }
                }
            };
        }
    }




    void testLayout()
    {   
        P<ScrollViewLayoutTesterBase> pThis = this;

        Size pixelSize = getPixelSize();

        SECTION("contentview null")
        {
            SECTION("no margin, no padding")
            {
                calcLayout( Size(1000, 1000) );

                verifyHorzBarVisible( false );
                verifyVertBarVisible( false );
                verifyContentViewBounds( Rect(0, 0, 1000, 1000) );
                verifyScrolledAreaSize( Size(1000, 1000)  );
                verifyViewPortSize( Size(1000, 1000) );
            }

            SECTION("padding")
            {
                getScrollView()->padding() = UiMargin(1,2,3,4);

                // the scrollview's margin should not influence the layout
                getScrollView()->margin() = UiMargin(5,6,7,8);

                CONTINUE_SECTION_WHEN_IDLE( pThis )
                {
                    pThis->calcLayout( Size(1000, 1000) );

                    pThis->verifyHorzBarVisible( false );
                    pThis->verifyVertBarVisible( false );
                    pThis->verifyContentViewBounds( Rect(4, 1, 1000-2-4, 1000-1-3) );
                    pThis->verifyScrolledAreaSize( Size(1000, 1000)  );
                    pThis->verifyViewPortSize( Size(1000, 1000) );
                };
            }
        }

        SECTION("contentview not null")
        {
            P< ScrollViewLayoutHelperTestContentView<Button> > pButton = newObj< ScrollViewLayoutHelperTestContentView<Button> >();

            // make sure that the button is not too small. The size of the content view determines the initial
            // size of the scroll view that many tests use as a starting point. And we want that initial size
            // to be bigger than the scroll bars would be, to ensure that the scroll bars are fully visible.
            // So to achieve that we set a multiline button label, which should cause the button to be bigger
            // than the scroll bars.
            pButton->label() = "Lorem ipsum\ndolor sit amet";

            getScrollView()->setContentView(pButton);

            Margin buttonMargin(1,2,3,4);
            pButton->margin() = UiMargin(buttonMargin.top, buttonMargin.right, buttonMargin.bottom, buttonMargin.left);

            Margin scrollViewPadding(5,6,7,8);
            getScrollView()->padding() = UiMargin(scrollViewPadding.top, scrollViewPadding.right, scrollViewPadding.bottom, scrollViewPadding.left);
        
            CONTINUE_SECTION_WHEN_IDLE(pThis, pButton, pixelSize, buttonMargin, scrollViewPadding)
            {   
                double horzBarHeight = pThis->getHorzBarHeight();
                double vertBarWidth = pThis->getVertBarWidth();

                Size optimalButtonSize = pButton->calcPreferredSize();
        
                Rect optimalButtonBounds( Point(4+8, 1+5), optimalButtonSize);

                // adjust the optimal size so that it is a multiple of the physical pixels
                optimalButtonBounds = pButton->adjustBounds( optimalButtonBounds, RoundType::nearest, RoundType::nearest );     
                optimalButtonSize = optimalButtonBounds.getSize();

                Size optimalSize = optimalButtonSize + buttonMargin + scrollViewPadding;                

                // calculate the adjusted optimal scrollview size, based on the optimal button bounds.
                {
                    Rect optimalScrollViewBounds( optimalButtonBounds );
                    optimalScrollViewBounds += buttonMargin + scrollViewPadding;
                    optimalScrollViewBounds = pThis->getScrollView()->adjustBounds( optimalScrollViewBounds, RoundType::nearest, RoundType::up );

                    optimalSize = Size(optimalScrollViewBounds.width, optimalScrollViewBounds.height);
                }

                
                int initialCalcPreferredSizeCallCount = pButton->getCalcPreferredSizeCallCount();

                SECTION("huge viewport")
                {
                    Size viewPortSize = optimalSize+Size(1000, 1000);

                    pThis->calcLayout( viewPortSize );
                    
                    // content view should be stretched to fill whole viewport
                    pThis->verifyHorzBarVisible( false );
                    pThis->verifyVertBarVisible( false );
                    pThis->verifyContentViewBounds( Rect(4+8, 1+5, viewPortSize.width - 2-4-6-8, viewPortSize.height -1-3-5-7) );
                    pThis->verifyScrolledAreaSize( viewPortSize  );
                    pThis->verifyViewPortSize( viewPortSize );
                    
                    // calcpreferredsize should have been called once
                    REQUIRE( pButton->getCalcPreferredSizeCallCount() == initialCalcPreferredSizeCallCount+1 );

                }
        
                SECTION("viewport matches needed content size")
                {
                    pThis->calcLayout( optimalSize );

                    pThis->verifyHorzBarVisible( false );
                    pThis->verifyVertBarVisible( false );
                    pThis->verifyContentViewBounds( optimalButtonBounds );
                    pThis->verifyScrolledAreaSize( optimalSize  );
                    pThis->verifyViewPortSize( optimalSize );
                    
                    // calcpreferredsize should have been called once
                    REQUIRE( pButton->getCalcPreferredSizeCallCount() == initialCalcPreferredSizeCallCount+1 );
                }

                SECTION("less width than needed, more than enough height for scrollbar")
                {
                    Size viewPortSize = optimalSize + Size(-pixelSize.width, pixelSize.height*1000);

                    pThis->calcLayout( viewPortSize );

                    pThis->verifyHorzBarVisible( pThis->_horzScrollingEnabled );
                    pThis->verifyVertBarVisible( false );

                    Rect expectedContentViewBounds;

                    if(pThis->_horzScrollingEnabled)
                        expectedContentViewBounds = Rect( optimalButtonBounds.getPosition(), optimalButtonBounds.getSize()+Size(0, pixelSize.height*1000-horzBarHeight) );
                    else
                    {
                        // no scrolling >= no scrollbar. Also width is truncated
                        expectedContentViewBounds = Rect( optimalButtonBounds.getPosition(), optimalButtonBounds.getSize()+Size(-pixelSize.width, pixelSize.height*1000) );
                    }                
                                    
                    // we allow slight deviation due to floating point calculations
                    pThis->verifyContentViewBounds( expectedContentViewBounds, 0.0001 );
                    

                    if(pThis->_horzScrollingEnabled)
                    {            
                        // scrollable area is the optimal width. The height is the viewport height minus the height of the horizontal scrollbar
                        pThis->verifyScrolledAreaSize( Size(optimalSize.width, viewPortSize.height-horzBarHeight) );
                        // horizontal scroll bar is visible, so the final viewport size should be smaller by that amount
                        pThis->verifyViewPortSize( viewPortSize-Size(0, horzBarHeight) );
                    }
                    else
                    {
                        // width is truncated.
                        pThis->verifyScrolledAreaSize( Size(optimalSize.width-pixelSize.width, viewPortSize.height) );
                        pThis->verifyViewPortSize( viewPortSize );
                    }

                    // calcpreferredsize should have been called at least once
                    REQUIRE( pButton->getCalcPreferredSizeCallCount() > initialCalcPreferredSizeCallCount );

                    if(!pThis->_horzScrollingEnabled)
                    {
                        if(pThis->_vertScrollingEnabled)
                        {
                            // reported available space should have been unlimited height (since scrollable) and 1 DIP less
                            // width than needed.
                            // The reported 
                            Size lastAvailSpace = pButton->getLastCalcPreferredSizeAvailableSpace();
                            Size expectedLastAvailSpace( optimalButtonSize.width-pixelSize.width, Size::componentNone());
                            REQUIRE( Dip::equal( lastAvailSpace, expectedLastAvailSpace ) );
                        }
                        else
                        {
                            // reported available space should have been the available height height (since not scrollable) and 1 DIP less
                            // width than needed.
                            Size lastAvailSpace = pButton->getLastCalcPreferredSizeAvailableSpace();
                            Size expectedLastAvailSpace = optimalButtonSize + Size(-pixelSize.width, pixelSize.height*1000);
                            REQUIRE( Dip::equal( lastAvailSpace, expectedLastAvailSpace ) );
                        }
                    }
                }

                SECTION("less width than needed, enough height for scrollbar")
                {
                    Size viewPortSize = optimalSize + Size(-pixelSize.width, horzBarHeight);

                    pThis->calcLayout( viewPortSize );

                    pThis->verifyHorzBarVisible( pThis->_horzScrollingEnabled );
                    pThis->verifyVertBarVisible( false );
                    
                    if(pThis->_horzScrollingEnabled)
                    {
                        pThis->verifyContentViewBounds( optimalButtonBounds );
                                    
                        // scrollable area is the optimal size
                        pThis->verifyScrolledAreaSize( optimalSize  );

                        // horizontal scroll bar is visible, so the final viewport size should be smaller by that amount
                        pThis->verifyViewPortSize( viewPortSize-Size(0, horzBarHeight) );
                    }
                    else
                    {
                        // content view should be expanded to fill the available height (since there is no scrollbar).
                        // width is truncated
                        pThis->verifyContentViewBounds( Rect( optimalButtonBounds.getPosition(), optimalButtonBounds.getSize()+Size(-pixelSize.width, horzBarHeight) ) );
                        pThis->verifyScrolledAreaSize( optimalSize+Size(-pixelSize.width, horzBarHeight) );

                        pThis->verifyViewPortSize( viewPortSize );
                    }
                }

                SECTION("less width than needed, not enough height for scrollbar")
                {
                    Size viewPortSize = optimalSize + Size(-pixelSize.width, horzBarHeight-pixelSize.height);

                    pThis->calcLayout( viewPortSize );

                    pThis->verifyHorzBarVisible( pThis->_horzScrollingEnabled );
                    pThis->verifyVertBarVisible( (pThis->_horzScrollingEnabled && pThis->_vertScrollingEnabled) );
                    
                    if(pThis->_horzScrollingEnabled && pThis->_vertScrollingEnabled)
                    {
                        // the horizontal scroll bar does not fit. So we should also get a vertical scrollbar                
                        pThis->verifyContentViewBounds( optimalButtonBounds );
            
                        // scrollable area is the optimal size
                        pThis->verifyScrolledAreaSize( optimalSize  );

                        // both scroll bars are visible, so the final viewport size should be smaller by that amount
                        pThis->verifyViewPortSize( viewPortSize-Size(vertBarWidth, horzBarHeight) );
                    }
                    else if(pThis->_horzScrollingEnabled)
                    {
                        // the viewport height is reduced due to the horizontal scroll bar. But vert scrolling is not allowed, so we won't
                        // get a vertical scrollbar. So the content height will be truncated to one less than what is needed.
                        pThis->verifyContentViewBounds( Rect(optimalButtonBounds.getPosition(), optimalButtonBounds.getSize()+Size(0,-pixelSize.height)) );
                                    
                        // scrollable area is the optimal width, since we can scroll. The height is optimal-pixelsize, since we need to truncate
                        pThis->verifyScrolledAreaSize( optimalSize+Size(0,-pixelSize.height)  );

                        // only one scroll bar is visible
                        pThis->verifyViewPortSize( viewPortSize-Size(0, horzBarHeight) );
                    }
                    else
                    {
                        // horizontal scrolling is disabled. vert scrolling is not needed, since without the horz scrollbar
                        // we have enough height. So there will be no scrolling. So it does not matter if vert scrolling is enabled or not.

                        // content fills the available space
                        pThis->verifyContentViewBounds( Rect(optimalButtonBounds.getPosition(), optimalButtonBounds.getSize()+Size(-pixelSize.width, horzBarHeight-pixelSize.height )) );
            
                        // scrolled area is extended / truncated to fit the available space
                        pThis->verifyScrolledAreaSize( optimalSize+Size(-pixelSize.height, horzBarHeight-pixelSize.height )  );

                        // viewport stays the same
                        pThis->verifyViewPortSize( viewPortSize );
                    }
                }

                SECTION("less height than needed, more than enough width for scrollbar")
                {
                    Size viewPortSize = optimalSize + Size(1000, -pixelSize.height);

                    pThis->calcLayout( viewPortSize );

                    pThis->verifyHorzBarVisible( false );
                    pThis->verifyVertBarVisible( pThis->_vertScrollingEnabled );


                    if(pThis->_vertScrollingEnabled)
                    {
                        pThis->verifyContentViewBounds( Rect( optimalButtonBounds.getPosition(), optimalButtonBounds.getSize()+Size(1000-vertBarWidth, 0) ) );
            
                        // scrollable area width is the viewport width minus the scrollbar width. Height is the optimal height
                        pThis->verifyScrolledAreaSize( Size(viewPortSize.width-vertBarWidth, optimalSize.height) );
                        // vertical scroll bar is visible, so the final viewport size should be smaller by that amount
                        pThis->verifyViewPortSize( viewPortSize-Size(vertBarWidth, 0) );
                    }
                    else
                    {
                        // cannot scroll vertically => no scrollbar shown.
                        // width fills the viewport. height is truncated to available space

                        pThis->verifyContentViewBounds( Rect( optimalButtonBounds.getPosition(), optimalButtonBounds.getSize()+Size(1000, -pixelSize.height) ) );
            
                        // scrollable area width is the viewport width. Height is the optimal height, truncated to the available space
                        pThis->verifyScrolledAreaSize( optimalSize + Size(1000,-pixelSize.height) );

                        // no scrollbar visible => full viewport size
                        pThis->verifyViewPortSize( viewPortSize );
                    }
                }


                SECTION("less height than needed, enough width for scrollbar")
                {
                    Size viewPortSize = optimalSize + Size(vertBarWidth, -pixelSize.height);

                    pThis->calcLayout( viewPortSize );

                    pThis->verifyHorzBarVisible( false );
                    pThis->verifyVertBarVisible( pThis->_vertScrollingEnabled );


                    if(pThis->_vertScrollingEnabled)
                    {
                        pThis->verifyContentViewBounds( optimalButtonBounds );
            
                        // scrollable area size is the optimal size
                        pThis->verifyScrolledAreaSize( optimalSize );
                        // vertical scroll bar is visible, so the final viewport size should be smaller by that amount
                        pThis->verifyViewPortSize( viewPortSize-Size(vertBarWidth, 0) );
                    }
                    else
                    {
                        // cannot scroll vertically => no scrollbar shown.
                        // width fills the viewport. height is truncated to available space

                        pThis->verifyContentViewBounds( Rect( optimalButtonBounds.getPosition(), optimalButtonBounds.getSize()+Size(vertBarWidth, -pixelSize.height) ) );
            
                        // scrollable area width is the viewport width. Height is the optimal height, truncated to the available space
                        pThis->verifyScrolledAreaSize( optimalSize + Size(vertBarWidth,-pixelSize.height) );

                        // no scrollbar visible => full viewport size
                        pThis->verifyViewPortSize( viewPortSize );
                    }
                }

                SECTION("less height than needed, not enough width for scrollbar")
                {
                    Size viewPortSize = optimalSize + Size(vertBarWidth-pixelSize.width, -pixelSize.height);

                    pThis->calcLayout( viewPortSize );

                    pThis->verifyHorzBarVisible( (pThis->_horzScrollingEnabled && pThis->_vertScrollingEnabled) );
                    pThis->verifyVertBarVisible( pThis->_vertScrollingEnabled );
                    
                    if(pThis->_horzScrollingEnabled && pThis->_vertScrollingEnabled)
                    {
                        pThis->verifyContentViewBounds( optimalButtonBounds );
            
                        // scrollable area size is the optimal size
                        pThis->verifyScrolledAreaSize( optimalSize );
                        // both scroll bars are visible, so the final viewport size should be smaller by that amount
                        pThis->verifyViewPortSize( viewPortSize-Size(vertBarWidth, horzBarHeight) );
                    }
                    else if(pThis->_vertScrollingEnabled)
                    {
                        // the viewport width is reduced due to the vertical scroll bar. But horz scrolling is not allowed, so we won't
                        // get a horizontal scrollbar. So the content width will be truncated to one less than what is needed.
                        pThis->verifyContentViewBounds( Rect(optimalButtonBounds.getPosition(), optimalButtonBounds.getSize()+Size(-pixelSize.width,0)) );
            
                        // scrollable area is the optimal height, since we can scroll. The width is optimal-pixelSize, since we need to truncate
                        pThis->verifyScrolledAreaSize( optimalSize+Size(-pixelSize.width,0) );

                        // only one scroll bar is visible
                        pThis->verifyViewPortSize( viewPortSize-Size(vertBarWidth, 0) );
                    }
                    else
                    {
                        // vertical scrolling is disabled. horizontal scrolling is not needed, since without the vertical scrollbar
                        // we have enough width. So there will be no scrolling. So it does not matter if horz scrolling is enabled or not.

                        // content fills the available space
                        pThis->verifyContentViewBounds( Rect(optimalButtonBounds.getPosition(), optimalButtonBounds.getSize()+Size(vertBarWidth-pixelSize.width, -pixelSize.height)) );
            
                        // scrolled area is extended / truncated to fit the available space
                        pThis->verifyScrolledAreaSize( optimalSize+Size(vertBarWidth-pixelSize.width, -pixelSize.height) );

                        // viewport stays the same
                        pThis->verifyViewPortSize( viewPortSize );
                    }                
                }

                SECTION("less width and less height than needed")
                {
                    Size viewPortSize = optimalSize + Size(-pixelSize.width, -pixelSize.height);

                    pThis->calcLayout( viewPortSize );

                    pThis->verifyHorzBarVisible( pThis->_horzScrollingEnabled );
                    pThis->verifyVertBarVisible( pThis->_vertScrollingEnabled );


                    if(pThis->_horzScrollingEnabled && pThis->_vertScrollingEnabled)
                    {
                        pThis->verifyContentViewBounds( optimalButtonBounds );
            
                        pThis->verifyScrolledAreaSize( optimalSize );
                        pThis->verifyViewPortSize( viewPortSize-Size(vertBarWidth, horzBarHeight) );
                    }
                    else if(pThis->_horzScrollingEnabled)
                    {
                        // we cannot scroll vertically. So height is truncated, width is optimal
                        pThis->verifyContentViewBounds( Rect(optimalButtonBounds.getPosition(), optimalButtonBounds.getSize()+Size(0, -pixelSize.height-horzBarHeight)) );
            
                        pThis->verifyScrolledAreaSize( optimalSize+Size(0,-pixelSize.height-horzBarHeight) );

                        // only one scrollbar is visible
                        pThis->verifyViewPortSize( viewPortSize-Size(0, horzBarHeight) );
                    }
                    else if(pThis->_vertScrollingEnabled)
                    {
                        // we cannot scroll horizontally. So width is truncated, height is optimal
                        pThis->verifyContentViewBounds( Rect(optimalButtonBounds.getPosition(), optimalButtonBounds.getSize()+Size(-pixelSize.width-vertBarWidth, 0)) );
            
                        pThis->verifyScrolledAreaSize( optimalSize+Size(-pixelSize.width-vertBarWidth, 0) );

                        // only one scrollbar is visible
                        pThis->verifyViewPortSize( viewPortSize-Size(vertBarWidth, 0) );
                    }
                    else
                    {
                        // no scrolling. Width and height are truncated
                        pThis->verifyContentViewBounds( Rect(optimalButtonBounds.getPosition(), optimalButtonBounds.getSize()+Size(-pixelSize.width,-pixelSize.height)) );
            
                        pThis->verifyScrolledAreaSize( optimalSize+Size(-pixelSize.width,-pixelSize.height) );

                        // only one scrollbar is visible
                        pThis->verifyViewPortSize( viewPortSize );
                    }
                }
            };
        }
    }

private:
        
    template<class BaseClass>
    class ScrollViewLayoutHelperTestContentView : public BaseClass
    {
    public:
    

        int getCalcPreferredSizeCallCount() const
        {
            return _calcPreferredSizeCallCount;
        }

        Size getLastCalcPreferredSizeAvailableSpace() const
        {
            return _lastCalcPreferredSizeAvailableSpace;
        }

        Size calcPreferredSize(const Size& availableSpace = Size::none()) const override
        {
            _lastCalcPreferredSizeAvailableSpace = availableSpace;
            _calcPreferredSizeCallCount++;

            return BaseClass::calcPreferredSize(availableSpace);
        }

    private:
        mutable int     _calcPreferredSizeCallCount = 0;
        mutable Size    _lastCalcPreferredSizeAvailableSpace;
    };

    
    
};



}
}

#endif