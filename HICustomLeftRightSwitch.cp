/*	File:		HICustomLeftRightSwitch.cp	Contains:	Demonstrates creating a simple custom left/right switch using the HIView APIs.	Version:	1.0.1	Disclaimer:	IMPORTANT:  This Apple software is supplied to you by Apple Computer, Inc.				("Apple") in consideration of your agreement to the following terms, and your				use, installation, modification or redistribution of this Apple software				constitutes acceptance of these terms.  If you do not agree with these terms,				please do not use, install, modify or redistribute this Apple software.				In consideration of your agreement to abide by the following terms, and subject				to these terms, Apple grants you a personal, non-exclusive license, under Apple�s				copyrights in this original Apple software (the "Apple Software"), to use,				reproduce, modify and redistribute the Apple Software, with or without				modifications, in source and/or binary forms; provided that if you redistribute				the Apple Software in its entirety and without modifications, you must retain				this notice and the following text and disclaimers in all such redistributions of				the Apple Software.  Neither the name, trademarks, service marks or logos of				Apple Computer, Inc. may be used to endorse or promote products derived from the				Apple Software without specific prior written permission from Apple.  Except as				expressly stated in this notice, no other rights or licenses, express or implied,				are granted by Apple herein, including but not limited to any patent rights that				may be infringed by your derivative works or by other works in which the Apple				Software may be incorporated.				The Apple Software is provided by Apple on an "AS IS" basis.  APPLE MAKES NO				WARRANTIES, EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION THE IMPLIED				WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY AND FITNESS FOR A PARTICULAR				PURPOSE, REGARDING THE APPLE SOFTWARE OR ITS USE AND OPERATION ALONE OR IN				COMBINATION WITH YOUR PRODUCTS.				IN NO EVENT SHALL APPLE BE LIABLE FOR ANY SPECIAL, INDIRECT, INCIDENTAL OR				CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE				GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)				ARISING IN ANY WAY OUT OF THE USE, REPRODUCTION, MODIFICATION AND/OR DISTRIBUTION				OF THE APPLE SOFTWARE, HOWEVER CAUSED AND WHETHER UNDER THEORY OF CONTRACT, TORT				(INCLUDING NEGLIGENCE), STRICT LIABILITY OR OTHERWISE, EVEN IF APPLE HAS BEEN				ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.	Copyright � 2002 Apple Computer, Inc., All Rights Reserved*/#include <Carbon/Carbon.h>#include "HICustomLeftRightSwitch.h"// structure in which we hold our custom push button's datatypedef struct	{	HIViewRef	view;							// the HIViewRef for our button	HIViewRef	leftLabelText;				// 2 statict text controls for my labels	HIViewRef	rightLabelText;	}CustomLeftRightSwitchData;#define kCustomLeftRightWidth			60#define kCustomLeftRightMargin		6#define kCustomLeftRightThumbWidth	8OSStatus CreateLabelSubButton(CustomLeftRightSwitchData* myData, ControlPartCode partCode);pascal OSStatus ViewHandler(EventHandlerCallRef inCaller, EventRef inEvent, void* inRefcon);CFStringRef GetCustomLeftRightSwitchClass();/* *  HICreateCustomLeftRightSwitch() *   *  Summary: *    Creates a left right switch control. *   *  Parameters: *     *    boundsRect: *      The bounding box of the control. *     *    leftLabel: *      The text for the label at the left of the switch. Can be NULL. *     *    rightLabel: *      The text for the label at the right of the switch. Can be NULL. *     *    allowMixedValue: *      If false, then the switch has only a value of left or right. *      If true, a mixed value "middle" is allowed. *      The maximum value of the control is set to 1 if false, 2 if true. *     *    outCustomLeftRightSwitch: *      On exit, contains the new control. *   *  Availability: *    Mac OS X:         in version 10.2 and later since it needs the HIView APIs *    CarbonLib:        not available *    Non-Carbon CFM:   not available */OSStatus HICreateCustomLeftRightSwitch(  WindowRef			theWind,  const HIRect *	boundsRect,  CFStringRef		leftLabel,							/* can be NULL */  CFStringRef		rightLabel,							/* can be NULL */  Boolean			allowMixedValue,  HIViewRef *		outCustomLeftRightSwitch)	{	HIObjectRef hiObject = NULL;	OSStatus theStatus = HIObjectCreate(GetCustomLeftRightSwitchClass(), 0, &hiObject);	if (theStatus != noErr) goto exitCreation;	// place the view into the Window content view	// we need to do this because CreateLabelSubButton needs a valid parent for our view.	HIViewRef root, contentView;	root = HIViewGetRoot(theWind);	HIViewFindByID(root, kHIViewWindowContentID, &contentView);	HIViewAddSubview(contentView, (HIViewRef)hiObject);			// position the view	theStatus = HIViewSetFrame((HIViewRef)hiObject, boundsRect);	if (theStatus != noErr) goto exitCreation;		// set the labels	if (leftLabel != NULL)		{		theStatus = SetControlData((HIViewRef)hiObject, kControlUpButtonPart, kControlStaticTextCFStringTag, sizeof(leftLabel), &leftLabel);		if (theStatus != noErr) goto exitCreation;		}	if (rightLabel != NULL)		{		theStatus = SetControlData((HIViewRef)hiObject, kControlDownButtonPart, kControlStaticTextCFStringTag, sizeof(rightLabel), &rightLabel);		if (theStatus != noErr) goto exitCreation;		}	// do we allow a mixed setting? if yes, then do nothing (default), else set the maximun value to 1.	if (!allowMixedValue) SetControlMaximum((HIViewRef)hiObject, 1);	exitCreation:	*outCustomLeftRightSwitch = (HIViewRef)hiObject;	return theStatus;	}/*----------------------------------------------------------------------------------------------------------*///	� CreateLabelSubButton//	Creates and initializes either left or right label "static Text" button (same UI as radios or checks)./*----------------------------------------------------------------------------------------------------------*/OSStatus CreateLabelSubButton(CustomLeftRightSwitchData* myData, ControlPartCode partCode)	{	OSStatus result;	ControlRef theLabel;	Rect frame = {10, 10, 20, 20};	ControlFontStyleRec theStyle;	theStyle.flags = kControlUseJustMask;	theStyle.just = (partCode == kControlUpButtonPart)?teJustRight:teJustLeft;	result = CreateStaticTextControl(NULL, &frame, NULL, &theStyle, &theLabel);		if (result == paramErr)		{		// most likely we got the 10.2, 10.2.1 bug where we can't		// call CreateStaticTextControl with a NULL window parameter		// let's create it in the window instead and move it later.				result = CreateStaticTextControl(GetControlOwner(myData->view), &frame, NULL, &theStyle, &theLabel);		}	if (theLabel == NULL) result = paramErr;	if (result != noErr) return result;		// adding our Static Text to our view's parent, thus making it our sibling	result = HIViewAddSubview(HIViewGetSuperview(myData->view), theLabel);	if (partCode == kControlUpButtonPart)		myData->leftLabelText = theLabel;	else		myData->rightLabelText = theLabel;		// the main view has to be first because static text don't handle ciicks	// or in other words, the main view has to be the first sibling	HIViewSetZOrder(myData->view, kHIViewZOrderAbove, NULL);	return result;	}/*----------------------------------------------------------------------------------------------------------*///	� GetCustomLeftRightSwitchClass//	Registers and returns an HIObject class for a custom left right switch button./*----------------------------------------------------------------------------------------------------------*/CFStringRef GetCustomLeftRightSwitchClass()	{		// following code is pretty much boiler plate.		static HIObjectClassRef	theClass;		if (theClass == NULL)		{		static EventTypeSpec kFactoryEvents[] =			{				// the next 3 messages are boiler plate								{ kEventClassHIObject, kEventHIObjectConstruct },				{ kEventClassHIObject, kEventHIObjectDestruct },				{ kEventClassHIObject, kEventHIObjectInitialize },									// the next messages are the actual minimum messages you need to				// implement a simple custom left right switch button:				//				// kEventControlHitTest has to be implemented so that you can				//      verify that the point passed in parameter is indeed in				//      an active part of your control.				//      Note: contrary to what you might think and what the name suggests				//            (HitTest), this message can be sent even when the mouse button				//            is not down. Do not assume that you just got a click.				//            The Control Manager is just asking you to verify if a point				//            is in a part of your control, nothing more.				//				// kEventControlHiliteChanged, you get this message if the user just clicked				//      in your control, or has left the scope of your control while the				//      button is still down. In each case, that means a change of hilite.				//      most of the time, we should just react by asking for a redraw.				//				// kEventControlHit, we need to change the value of our control when we				//      detect a succesful track.				//				// kEventControlValueFieldChanged, the control value has changed. either after				//      a succesful track detected by kEventControlHit or because our client				//      called SetControlValue, we should just react by asking for a redraw.				//				// kEventControlDraw, you need to draw your control (or part of it),				//      according to its state.				//				// kEventControlSetData, we need to override this message in order to be able				//      to setup/modify our left and right labels.				//				// kEventControlBoundsChanged, we need to override this message in order to be				//      able to move our left and right labesl along with our widget.									{ kEventClassControl, kEventControlHitTest },				{ kEventClassControl, kEventControlHit },				{ kEventClassControl, kEventControlHiliteChanged },				{ kEventClassControl, kEventControlValueFieldChanged },				{ kEventClassControl, kEventControlDraw },				{ kEventClassControl, kEventControlSetData },				{ kEventClassControl, kEventControlBoundsChanged }			};				HIObjectRegisterSubclass(kCustomLeftRightSwitchClassID, kHIViewClassID, 0, ViewHandler,								  GetEventTypeCount(kFactoryEvents), kFactoryEvents, 0, &theClass);		}		return kCustomLeftRightSwitchClassID;	}/*----------------------------------------------------------------------------------------------------------*///	� ViewHandler//	Event handler that implements our custom left right switch button./*----------------------------------------------------------------------------------------------------------*/pascal OSStatus ViewHandler(EventHandlerCallRef inCaller, EventRef inEvent, void* inRefcon)	{	OSStatus result = eventNotHandledErr;	CustomLeftRightSwitchData*	myData = (CustomLeftRightSwitchData*)inRefcon;	switch (GetEventClass(inEvent))		{		case kEventClassHIObject:			switch (GetEventKind(inEvent))				{				case kEventHIObjectConstruct:					{					// allocate some instance data					CustomLeftRightSwitchData* myData = (CustomLeftRightSwitchData*) calloc(1, sizeof(CustomLeftRightSwitchData));										// get our superclass instance					HIViewRef epView;					GetEventParameter(inEvent, kEventParamHIObjectInstance, typeHIObjectRef, NULL, sizeof(epView), NULL, &epView);										// remember our superclass in our instance data and initialize other fields					myData->view = epView;					myData->leftLabelText = NULL;					myData->rightLabelText = NULL;										// store our instance data into the event					result = SetEventParameter(inEvent, kEventParamHIObjectInstance, typeVoidPtr, sizeof(myData), &myData);					break;					}									case kEventHIObjectDestruct:					{					free(myData);					result = noErr;					break;					}				case kEventHIObjectInitialize:					{					// always begin kEventHIObjectInitialize by calling through to the previous handler					result = CallNextEventHandler(inCaller, inEvent);										// if that succeeded, do our own initialization					if (result == noErr)						{						SetControlMinimum(myData->view, 0);												// default behavior is to allow mixed setting						SetControlMaximum(myData->view, 2);						}					break;					}								default:					break;				}			break;					case kEventClassControl:			switch (GetEventKind(inEvent))				{				//	Draw the view.				case kEventControlDraw:					{					CGContextRef	context;					HIRect			bounds;										result = GetEventParameter(inEvent, kEventParamCGContextRef, typeCGContextRef, NULL, sizeof(context), NULL, &context);					if (result != noErr) {DebugStr("\pGetEventParameter failed for kEventControlDraw"); break;}										HIViewGetBounds(myData->view, &bounds);										// compute widget bounds					bounds.origin.x += (bounds.size.width - kCustomLeftRightWidth) / 2;					bounds.origin.x += kCustomLeftRightMargin;										bounds.size.width = kCustomLeftRightWidth - kCustomLeftRightMargin - kCustomLeftRightMargin;					// and draw the widget according to hilite state					if ((!IsControlHilited(myData->view)) || (!IsControlActive(myData->view)))						CGContextSetGrayFillColor(context, 0.5, 0.3);					else						CGContextSetRGBFillColor(context, 0.1, 0.1, 1.0, 0.3);					CGContextFillRect(context, bounds);										// compute the "thumb" widget bounds using the current value					SInt16 curVal = GetControlValue(myData->view);					if (curVal == 0) bounds.origin.x += kCustomLeftRightMargin;					else if (curVal == 1) bounds.origin.x += (bounds.size.width - kCustomLeftRightThumbWidth - kCustomLeftRightMargin);					else bounds.origin.x += (kCustomLeftRightWidth - kCustomLeftRightMargin - kCustomLeftRightMargin - kCustomLeftRightThumbWidth) / 2;					bounds.size.width = kCustomLeftRightThumbWidth;					bounds.origin.y += kCustomLeftRightThumbWidth / 2;					bounds.size.height -= kCustomLeftRightThumbWidth;										// and draw the "thumb" widget					CGContextSetRGBFillColor(context, 0.1, 0.1, 0.1, 0.3);					CGContextFillRect(context, bounds);										result = noErr;					break;					}				//	Determine if a point is in the view.				case kEventControlHitTest:					{					HIPoint	pt;					HIRect	bounds;										// the point parameter is in view-local coords.					GetEventParameter(inEvent, kEventParamMouseLocation, typeHIPoint, NULL, sizeof(pt), NULL, &pt);					HIViewGetBounds(myData->view, &bounds);										// compute our part bounds and test					if (CGRectContainsPoint(bounds, pt))						{						ControlPartCode part;						HIRect labelBounds = bounds;						labelBounds.size.width -= kCustomLeftRightWidth;						labelBounds.size.width /= 2;						if (CGRectContainsPoint(labelBounds, pt)) part = kControlUpButtonPart;						else							{							labelBounds.origin.x += (bounds.size.width - labelBounds.size.width);							if (CGRectContainsPoint(labelBounds, pt))								part = kControlDownButtonPart;							else								part = kControlCheckBoxPart;							}						SetEventParameter(inEvent, kEventParamControlPart, typeControlPartCode, sizeof(part), &part);						result = noErr;						}					break;					}				//	React to hilite changes by invalidating the view so that it will be redrawn.				case kEventControlHiliteChanged:					HIViewSetNeedsDisplay(myData->view, true);					break;				//	React to value changes by invalidating the view so that it will be redrawn.				case kEventControlValueFieldChanged:					HIViewSetNeedsDisplay(myData->view, true);					result = noErr;					break;				//	We change the control value according to what happened and where.				// We leave result set to eventNotHandledErr so that a hit event will				// continue to be processed and the Control Manager eventually will send				// the associated command ID to the command process hanler, except if we				// detect that the value shouldn't change (ie. clicking on the "Right"				// label when the thumb is already in the "Right" position, and then we				// return noErr to stop the processing.				case kEventControlHit:					{					ControlPartCode part;					GetEventParameter(inEvent, kEventParamControlPart, typeControlPartCode, NULL, sizeof(part), NULL, &part);					// click on our Left label?					if (part == kControlUpButtonPart)						{						if (GetControlValue(myData->view) != 0)							{							SetControlValue(myData->view, 0);							}						else result = noErr;						}										// click on our Right label?					else if (part == kControlDownButtonPart)						{						if (GetControlValue(myData->view) != 1)							{							SetControlValue(myData->view, 1);							}						else result = noErr;						}										// click on widget?					else if (part == kControlCheckBoxPart)						{						SInt16 curVal = GetControlValue(myData->view);						SetControlValue(myData->view, curVal+1);												// Were we at maximum value already, whichever was allowed (1 or 2)?						if (curVal == GetControlValue(myData->view))							// yes, so go back to 0.							SetControlValue(myData->view, 0);						}					break;					}				//	Needs to propagate the bounds changed message to our labels subcontrols.				case kEventControlBoundsChanged:					{					// get the new bounds in HIRect format					HIRect newHIBounds;					GetEventParameter(inEvent, kEventParamCurrentBounds, typeHIRect, NULL, sizeof(newHIBounds), NULL, &newHIBounds);										// compute our new bounds for our Left and Right labels					HIRect leftLabelBounds = newHIBounds;					leftLabelBounds.size.width -= kCustomLeftRightWidth;					leftLabelBounds.size.width /= 2;					HIRect rightLabelBounds = leftLabelBounds;					rightLabelBounds.origin.x += (newHIBounds.size.width - leftLabelBounds.size.width);										// if our StaticText labels are not instantiated yet, let's create them now					result = noErr;					if (myData->leftLabelText == NULL)						result = CreateLabelSubButton(myData, kControlUpButtonPart);					if (result == noErr)						if (myData->rightLabelText == NULL)							result = CreateLabelSubButton(myData, kControlDownButtonPart);					if (result != noErr) break;										// and set their frame to their new values					HIViewSetFrame(myData->leftLabelText, &leftLabelBounds);					HIViewSetFrame(myData->rightLabelText, &rightLabelBounds);										// and ask for a redraw					HIViewSetNeedsDisplay(myData->view, true);					result = eventNotHandledErr;					break;					}				//	If the control part is kControlUpButtonPart or kControlDownButtonPart then absorb the message				// else return eventNotHandledErr.				case kEventControlSetData:					{					ControlPartCode partCode;					GetEventParameter(inEvent, kEventParamControlPart, typeControlPartCode, NULL, sizeof(partCode), NULL, &partCode);										// we're only interested in passing SetControlData info to our Left and Right labels					if ((partCode == kControlUpButtonPart) || (partCode == kControlDownButtonPart))						{						result = noErr;						HIViewRef theLabel = (partCode == kControlUpButtonPart)?myData->leftLabelText:myData->rightLabelText;												// if our StaticText labels are not instantiated yet, let's create them now						if (theLabel == NULL)							{							result = CreateLabelSubButton(myData, partCode);							theLabel = (partCode == kControlUpButtonPart)?myData->leftLabelText:myData->rightLabelText;							}						if (theLabel == NULL) result = paramErr;						if (result != noErr) break;												// and extract the tag, size, and pointer parameters						ResType theTag;						Size theSize;						void * theData;						GetEventParameter(inEvent, kEventParamControlDataTag, typeEnumeration, NULL, sizeof(theTag), NULL, &theTag);						GetEventParameter(inEvent, kEventParamControlDataBufferSize, typeLongInteger, NULL, sizeof(theSize), NULL, &theSize);						GetEventParameter(inEvent, kEventParamControlDataBuffer, typePtr, NULL, sizeof(theData), NULL, &theData);												// to pass them to our sub control						result = SetControlData(theLabel, kControlEntireControl, theTag, theSize, theData);						HIViewSetNeedsDisplay(myData->view, true);						}					else result = eventNotHandledErr;					break;					}								default:					break;				}			break;					default:			break;		}		return result;	}