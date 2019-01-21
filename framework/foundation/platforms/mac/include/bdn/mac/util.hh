#pragma once

#import <AppKit/AppKit.h>
#import <bdn/foundationkit/stringUtil.hh>
#include <Availability.h>

#include <bdn/constants.h>

namespace bdn
{
    namespace mac
    {

        /** Converts a mac rect to a bdn::Rect.

            Mac UI objects often use an inverted coordinate space with the
           origin in the lower left corner, rather than the lower right.

            This function can convert from the inverted space to "normal" space
           with top left being the origin. To do that you must set
           coordinateSpaceHeightForFlipping to the full height of the coordinate
           space (i.e. the height of the area that the rect is in).

            If coordinateSpaceHeightForFlipping is -1 then no flipping is done.
            */
        inline Rect macRectToRect(const NSRect &rect, int coordinateSpaceHeightForFlipping)
        {
            // the mac uses DIPs, just like us. So no conversion necessary,
            // except possibly for coordinate flipping

            double y = rect.origin.y;

            if (coordinateSpaceHeightForFlipping != -1) {
                // when we need to flip then we need to adjust for two things:
                // 1) the origin of the mac coordinate space is the bottom left
                // 2) the "position" of the rect refers to the lower left corner
                // of
                //    the rect.

                // Adjust for 1)
                y = coordinateSpaceHeightForFlipping - y;

                // Adjust for 2)
                y -= rect.size.height;
            }

            return Rect(rect.origin.x, y, rect.size.width, rect.size.height);
        }

        /** Converts a bdn::Rect to a mac rect.

            Mac UI objects often use an inverted coordinate space with the
           origin in the lower left corner, rather than the lower right.

            This function can convert from the "normal" space with
            top left being the origin to the inverted mac space.
            To do that you must set coordinateSpaceHeightForFlipping
            to the full height of the coordinate space (i.e. the height of the
           area that the rect is in).

            If coordinateSpaceHeightForFlipping is -1 then no flipping is done.
            */
        inline NSRect rectToMacRect(const Rect &rect, int coordinateSpaceHeightForFlipping)
        {
            // the mac uses DIPs, just like us. So no conversion necessary,
            // except possibly for coordinate flipping

            NSRect macRect;

            macRect.origin.x = rect.x;
            macRect.origin.y = rect.y;
            macRect.size.width = rect.width;
            macRect.size.height = rect.height;

            if (coordinateSpaceHeightForFlipping != -1) {
                // see macRectToRect for an explanation.

                macRect.origin.y += macRect.size.height;
                macRect.origin.y = coordinateSpaceHeightForFlipping - macRect.origin.y;
            }

            return macRect;
        }

        inline Size macSizeToSize(const NSSize &macSize)
        {
            // the mac uses DIPs, just like us. So no conversion necessary.

            return Size(macSize.width, macSize.height);
        }

        inline NSSize sizeToMacSize(const Size &size)
        {
            // the mac uses DIPs, just like us. So no conversion necessary.

            NSSize macSize;
            macSize.width = size.width;
            macSize.height = size.height;

            return macSize;
        }

        inline NSString *stringToMacString(const String &val) { return bdn::fk::stringToNSString(val); }

        inline String macStringToString(NSString *nsString) { return bdn::fk::nsStringToString(nsString); }

#if defined(MAC_OS_X_VERSION_10_13) && MAC_OS_X_VERSION_MIN_REQUIRED >= MAC_OS_X_VERSION_10_13

        inline NSControlStateValue triStateToNSControlStateValue(TriState state)
            __OSX_AVAILABLE_STARTING(__MAC_10_13, __IPHONE_NA)
        {
            switch (state) {
            case TriState::on:
                return NSControlStateValueOn;
            case TriState::off:
                return NSControlStateValueOff;
            case TriState::mixed:
                return NSControlStateValueMixed;
            }
            return NSControlStateValueOff;
        }

        inline TriState nsControlStateValueToTriState(NSControlStateValue nsState)
            __OSX_AVAILABLE_STARTING(__MAC_10_13, __IPHONE_NA)
        {
            switch (nsState) {
            case NSControlStateValueOn:
                return TriState::on;
            case NSControlStateValueOff:
                return TriState::off;
            case NSControlStateValueMixed:
                return TriState::mixed;
            }
            return TriState::off;
        }

#else
        inline NSInteger triStateToNSControlStateValue(TriState state)
        {
            switch (state) {
            case TriState::on:
                return NSOnState;
            case TriState::off:
                return NSOffState;
            case TriState::mixed:
                return NSMixedState;
            }
            return NSOffState;
        }

        inline TriState nsControlStateValueToTriState(NSInteger state)
        {
            switch (state) {
            case NSOnState:
                return TriState::on;
            case NSOffState:
                return TriState::off;
            case NSMixedState:
                return TriState::mixed;
            }
            return TriState::off;
        }
#endif
    }
}
