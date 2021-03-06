/*   Bridge Command 5.0 Ship Simulator
     Copyright (C) 2014 James Packer

     This program is free software; you can redistribute it and/or modify
     it under the terms of the GNU General Public License version 2 as
     published by the Free Software Foundation

     This program is distributed in the hope that it will be useful,
     but WITHOUT ANY WARRANTY; without even the implied warranty of
     MERCHANTABILITY Or FITNESS For A PARTICULAR PURPOSE.  See the
     GNU General Public License For more details.

     You should have received a copy of the GNU General Public License along
     with this program; if not, write to the Free Software Foundation, Inc.,
     51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA. */

#include "MyEventReceiver.hpp"

#include <iostream>
#include <string>

#include "GUIMain.hpp"
#include "SimulationModel.hpp"
#include "Utilities.hpp"

using namespace irr;

    MyEventReceiver::MyEventReceiver(irr::IrrlichtDevice* dev, SimulationModel* model, GUIMain* gui, irr::u32 portJoystickAxis, irr::u32 stbdJoystickAxis, irr::u32 rudderJoystickAxis, irr::u32 portJoystickNo, irr::u32 stbdJoystickNo, irr::u32 rudderJoystickNo, std::vector<std::string>* logMessages) //Constructor
	{
		this->model = model; //Link to the model
		this->gui = gui; //Link to GUI (Not currently used, all comms through model)
		scrollBarPosSpeed = 0;
		scrollBarPosHeading = 0;

		//store device
		device = dev;

		//set up joystick if present, and inform user what's available
		dev->activateJoysticks(joystickInfo);

		//Tell user about joysticks via the log
		dev->getLogger()->log(""); //add a blank line
		std::string joystickInfoMessage = "Number of joysticks detected: ";
		joystickInfoMessage.append(std::string(core::stringc(joystickInfo.size()).c_str()));
		dev->getLogger()->log(joystickInfoMessage.c_str());
        for(unsigned int i = 0; i<joystickInfo.size(); i++) {
            //Print out name and number of each joystick
            joystickInfoMessage = "Joystick number: ";
            joystickInfoMessage.append(core::stringc(i).c_str());
            joystickInfoMessage.append(", Name: ");
            joystickInfoMessage.append(std::string(joystickInfo[i].Name.c_str()));
            dev->getLogger()->log(joystickInfoMessage.c_str());
        }
        dev->getLogger()->log(""); //add a blank line

		this->portJoystickAxis=portJoystickAxis;
		this->stbdJoystickAxis=stbdJoystickAxis;
		this->rudderJoystickAxis=rudderJoystickAxis;
		this->portJoystickNo=portJoystickNo;
		this->stbdJoystickNo=stbdJoystickNo;
		this->rudderJoystickNo=rudderJoystickNo;

		//Indicate that previous joystick information hasn't been initialised
		previousJoystickPort = INFINITY;
		previousJoystickStbd = INFINITY;
		previousJoystickRudder = INFINITY;

		this->logMessages = logMessages;

        //assume mouse buttons not pressed initially
        leftMouseDown = false;
        rightMouseDown = false;
	}

    bool MyEventReceiver::OnEvent(const SEvent& event)
	{

        //From log
        if (event.EventType == EET_LOG_TEXT_EVENT) {
            //Store these in a global log.
            std::string eventText(event.LogEvent.Text);
            logMessages->push_back(eventText);
            return true;
        }

        //From mouse - keep track of button press state
        if (event.EventType == EET_MOUSE_INPUT_EVENT) {
            if (event.MouseInput.Event == EMIE_LMOUSE_PRESSED_DOWN ) {leftMouseDown=true;}
            if (event.MouseInput.Event == EMIE_LMOUSE_LEFT_UP ) {leftMouseDown=false;}
            if (event.MouseInput.Event == EMIE_RMOUSE_PRESSED_DOWN ) {rightMouseDown=true;}
            if (event.MouseInput.Event == EMIE_RMOUSE_LEFT_UP ) {rightMouseDown=false;}
            model->setMouseDown(leftMouseDown || rightMouseDown); //Set if either mouse is down
        }

        if (event.EventType == EET_GUI_EVENT)
		{
			s32 id = event.GUIEvent.Caller->getID();
            if (event.GUIEvent.EventType==gui::EGET_SCROLL_BAR_CHANGED)
            {

               if (id == GUIMain::GUI_ID_HEADING_SCROLL_BAR)
                  {
                      scrollBarPosHeading = ((gui::IGUIScrollBar*)event.GUIEvent.Caller)->getPos();
                      model->setHeading(scrollBarPosHeading);
                  }

              if (id == GUIMain::GUI_ID_SPEED_SCROLL_BAR)
                  {
                        scrollBarPosSpeed = ((gui::IGUIScrollBar*)event.GUIEvent.Caller)->getPos();
                        model->setSpeed(scrollBarPosSpeed);
                  }

              if (id == GUIMain::GUI_ID_STBD_SCROLL_BAR)
                  {
                        model->setStbdEngine(((gui::IGUIScrollBar*)event.GUIEvent.Caller)->getPos()/-100.0); //Convert to from +-100 to +-1, and invert up/down
                        //If right mouse button, set the other engine as well
                        if (rightMouseDown) {
                            model->setPortEngine(((gui::IGUIScrollBar*)event.GUIEvent.Caller)->getPos()/-100.0);
                        }
                  }
              if (id == GUIMain::GUI_ID_PORT_SCROLL_BAR)
                  {
                        model->setPortEngine(((gui::IGUIScrollBar*)event.GUIEvent.Caller)->getPos()/-100.0); //Convert to from +-100 to +-1, and invert up/down
                        //If right mouse button, set the other engine as well
                        if (rightMouseDown) {
                            model->setStbdEngine(((gui::IGUIScrollBar*)event.GUIEvent.Caller)->getPos()/-100.0);
                        }
                  }
              if (id == GUIMain::GUI_ID_RUDDER_SCROLL_BAR)
                  {
                        model->setRudder(((gui::IGUIScrollBar*)event.GUIEvent.Caller)->getPos());
                  }
              if (id == GUIMain::GUI_ID_RADAR_GAIN_SCROLL_BAR)
                  {
                        model->setRadarGain(((gui::IGUIScrollBar*)event.GUIEvent.Caller)->getPos());
                  }
              if (id == GUIMain::GUI_ID_RADAR_CLUTTER_SCROLL_BAR)
                  {
                        model->setRadarClutter(((gui::IGUIScrollBar*)event.GUIEvent.Caller)->getPos());
                  }
              if (id == GUIMain::GUI_ID_RADAR_RAIN_SCROLL_BAR)
                  {
                        model->setRadarRain(((gui::IGUIScrollBar*)event.GUIEvent.Caller)->getPos());
                  }
              if (id == GUIMain::GUI_ID_WEATHER_SCROLL_BAR)
                  {
                        model->setWeather(((gui::IGUIScrollBar*)event.GUIEvent.Caller)->getPos()/10.0); //Scroll bar 0-120, weather 0-12
                  }
              if (id == GUIMain::GUI_ID_RAIN_SCROLL_BAR)
                  {
                        model->setRain(((gui::IGUIScrollBar*)event.GUIEvent.Caller)->getPos()/10.0); //Scroll bar 0-100, rain 0-10
                  }
              if (id == GUIMain::GUI_ID_VISIBILITY_SCROLL_BAR)
                  {
                        model->setVisibility(((gui::IGUIScrollBar*)event.GUIEvent.Caller)->getPos()/10.0); //Scroll bar 1-101, vis 0.1-10.1
                  }
            }

            if (event.GUIEvent.EventType==gui::EGET_MESSAGEBOX_OK) {
                if (id == GUIMain::GUI_ID_CLOSE_BOX) {
                    device->closeDevice(); //Confirm shutdown.
                }
            }

            if (event.GUIEvent.EventType==gui::EGET_BUTTON_CLICKED) {
                if (id == GUIMain::GUI_ID_START_BUTTON)
                {
                    model->setAccelerator(1.0);
                }

                if (id == GUIMain::GUI_ID_RADAR_INCREASE_BUTTON)
                {
                    model->increaseRadarRange();
                }

                if (id == GUIMain::GUI_ID_RADAR_DECREASE_BUTTON)
                {
                    model->decreaseRadarRange();
                }

                if (id == GUIMain::GUI_ID_BIG_RADAR_BUTTON)
                {
                    gui->setLargeRadar(true);
                    model->setRadarDisplayRadius(gui->getRadarPixelRadius());
                    gui->hide2dInterface();
                }

                if (id == GUIMain::GUI_ID_SMALL_RADAR_BUTTON)
                {
                    gui->setLargeRadar(false);
                    model->setRadarDisplayRadius(gui->getRadarPixelRadius());
                    gui->show2dInterface();
                }

                if (id == GUIMain::GUI_ID_SHOW_INTERFACE_BUTTON)
                {
                    gui->show2dInterface();
                }

                if (id == GUIMain::GUI_ID_HIDE_INTERFACE_BUTTON)
                {
                    gui->hide2dInterface();
                }

                if (id == GUIMain::GUI_ID_BINOS_INTERFACE_BUTTON)
                {
                    model->setZoom(((gui::IGUIButton*)event.GUIEvent.Caller)->isPressed());
                }

                if (id == GUIMain::GUI_ID_RADAR_EBL_LEFT_BUTTON)
                {
                    model->decreaseRadarEBLBrg();
                }

                if (id == GUIMain::GUI_ID_RADAR_EBL_RIGHT_BUTTON)
                {
                    model->increaseRadarEBLBrg();
                }

                if (id == GUIMain::GUI_ID_RADAR_EBL_UP_BUTTON)
                {
                    model->increaseRadarEBLRange();
                }

                if (id == GUIMain::GUI_ID_RADAR_EBL_DOWN_BUTTON)
                {
                    model->decreaseRadarEBLRange();
                }

                //Radar mode buttons
                if (id == GUIMain::GUI_ID_RADAR_NORTH_BUTTON)
                {
                    model->setRadarNorthUp();
                }
                if (id == GUIMain::GUI_ID_RADAR_COURSE_BUTTON)
                {
                    model->setRadarCourseUp();
                }
                if (id == GUIMain::GUI_ID_RADAR_HEAD_BUTTON)
                {
                    model->setRadarHeadUp();
                }

                if (id == GUIMain::GUI_ID_SHOW_LOG_BUTTON)
                {
                    gui->showLogWindow();
                }

            } //Button clicked

            if (event.GUIEvent.EventType == gui::EGET_COMBO_BOX_CHANGED) {

                if (id == GUIMain::GUI_ID_ARPA_TRUE_REL_BOX || id == GUIMain::GUI_ID_BIG_ARPA_TRUE_REL_BOX)
                {
                    s32 selected = ((gui::IGUIComboBox*)event.GUIEvent.Caller)->getSelected();
                    if(selected == 0) {
                        model->setRadarARPATrue();
                    } else if (selected == 1) {
                        model->setRadarARPARel();
                    }

                    //Set both linked inputs - brute force
                    gui::IGUIElement* other = device->getGUIEnvironment()->getRootGUIElement()->getElementFromId(GUIMain::GUI_ID_ARPA_TRUE_REL_BOX,true);
                    if(other!=0) {
                        ((gui::IGUIComboBox*)other)->setSelected(((gui::IGUIComboBox*)event.GUIEvent.Caller)->getSelected());
                    }
                    other = device->getGUIEnvironment()->getRootGUIElement()->getElementFromId(GUIMain::GUI_ID_BIG_ARPA_TRUE_REL_BOX,true);
                    if(other!=0) {
                        ((gui::IGUIComboBox*)other)->setSelected(((gui::IGUIComboBox*)event.GUIEvent.Caller)->getSelected());
                    }

                }

            }//Combo box

            if ((id==GUIMain::GUI_ID_ARPA_ON_BOX || id==GUIMain::GUI_ID_BIG_ARPA_ON_BOX) && event.GUIEvent.EventType == gui::EGET_CHECKBOX_CHANGED) {
                //ARPA on/off checkbox
                bool boxState = ((gui::IGUICheckBox*)event.GUIEvent.Caller)->isChecked();
                model->setArpaOn(boxState);

                //Set both linked inputs - brute force
                gui::IGUIElement* other = device->getGUIEnvironment()->getRootGUIElement()->getElementFromId(GUIMain::GUI_ID_ARPA_ON_BOX,true);
                if(other!=0) {
                    ((gui::IGUICheckBox*)other)->setChecked(boxState);
                }
                other = device->getGUIEnvironment()->getRootGUIElement()->getElementFromId(GUIMain::GUI_ID_BIG_ARPA_ON_BOX,true);
                if(other!=0) {
                    ((gui::IGUICheckBox*)other)->setChecked(boxState);
                }

            }

            if ( (id==GUIMain::GUI_ID_ARPA_VECTOR_TIME_BOX || id==GUIMain::GUI_ID_BIG_ARPA_VECTOR_TIME_BOX) && (event.GUIEvent.EventType == gui::EGET_EDITBOX_ENTER || event.GUIEvent.EventType == gui::EGET_ELEMENT_FOCUS_LOST ) ) {
                std::wstring boxWString = std::wstring(((gui::IGUIEditBox*)event.GUIEvent.Caller)->getText());
                std::string boxString(boxWString.begin(), boxWString.end());
                f32 value = Utilities::lexical_cast<f32>(boxString);

                if (value > 0 && value <= 60) {
                    model->setRadarARPAVectors(value);
                } else {
                    event.GUIEvent.Caller->setText(L"Invalid");
                }

                //Set both linked inputs - brute force
                gui::IGUIElement* other = device->getGUIEnvironment()->getRootGUIElement()->getElementFromId(GUIMain::GUI_ID_ARPA_VECTOR_TIME_BOX,true);
                if(other!=0) {
                    other->setText(event.GUIEvent.Caller->getText());
                }
                other = device->getGUIEnvironment()->getRootGUIElement()->getElementFromId(GUIMain::GUI_ID_BIG_ARPA_VECTOR_TIME_BOX,true);
                if(other!=0) {
                    other->setText(event.GUIEvent.Caller->getText());
                }
            }


        } //GUI Event


        //From keyboard
        if (event.EventType == EET_KEY_INPUT_EVENT && event.KeyInput.PressedDown) {
            //Check here that there isn't focus on a GUI edit box. If we are, don't process key inputs here.
            gui::IGUIElement* focussedElement = device->getGUIEnvironment()->getFocus();
            if ( !(focussedElement && focussedElement->getType()==gui::EGUIET_EDIT_BOX)) {

                if (event.KeyInput.Shift) {
                    //Shift down

                } else if (event.KeyInput.Control) {
                    //Ctrl down

                    switch(event.KeyInput.Key)
                    {
                        //Camera look
                        case KEY_UP:
                            model->lookAhead();
                            break;
                        case KEY_DOWN:
                            model->lookAstern();
                            break;
                        case KEY_LEFT:
                            model->lookPort();
                            break;
                        case KEY_RIGHT:
                            model->lookStbd();
                            break;
                        default:
                            //don't do anything
                            break;
                    }

                } else {
                    //Shift and Ctrl not down

                    switch(event.KeyInput.Key)
                    {
                        //Accelerator
                        case KEY_KEY_0:
                            model->setAccelerator(0.0);
                            break;
                        case KEY_RETURN:
                            model->setAccelerator(1.0);
                            break;
                        case KEY_KEY_1:
                            model->setAccelerator(1.0);
                            break;
                        case KEY_KEY_2:
                            model->setAccelerator(2.0);
                            break;
                        case KEY_KEY_3:
                            model->setAccelerator(5.0);
                            break;
                        case KEY_KEY_4:
                            model->setAccelerator(15.0);
                            break;
                        case KEY_KEY_5:
                            model->setAccelerator(30.0);
                            break;
                        case KEY_KEY_6:
                            model->setAccelerator(60.0);
                            break;
                        case KEY_KEY_7:
                            model->setAccelerator(3600.0);
                            break;

                        //Camera look
                        case KEY_UP:
                            model->lookUp();
                            break;
                        case KEY_DOWN:
                            model->lookDown();
                            break;
                        case KEY_LEFT:
                            model->lookLeft();
                            break;
                        case KEY_RIGHT:
                            model->lookRight();
                            break;
                        case KEY_SPACE:
                            model->changeView();
                            break;

                        //toggle full screen 3d
                        case KEY_KEY_F:
                            gui->toggleShow2dInterface();
                            break;

                        //Quit with esc or F4 (for alt-F4)
                        case KEY_ESCAPE:
                        case KEY_F4:
                            model->setAccelerator(0.0);
                            device->sleep(500);
                            device->clearSystemMessages();
                            device->getGUIEnvironment()->addMessageBox(L"Quit?",L"Quit?",true,gui::EMBF_OK|gui::EMBF_CANCEL,0,GUIMain::GUI_ID_CLOSE_BOX);//I18n
                            break;

                        default:
                            //don't do anything
                            break;
                    }
                }
            }
		}

		//From joystick (actually polled, once per run():
        if (event.EventType == EET_JOYSTICK_INPUT_EVENT) {


            irr::f32 newJoystickPort = previousJoystickPort;
            irr::f32 newJoystickStbd = previousJoystickStbd;
            irr::f32 newJoystickRudder = previousJoystickRudder;

            u8 thisJoystick = event.JoystickEvent.Joystick;
            for (u8 thisAxis = 0; thisAxis < event.JoystickEvent.NUMBER_OF_AXES; thisAxis++) {

                //Check which type we correspond to
                if (thisJoystick == portJoystickNo && thisAxis == portJoystickAxis) {
                    newJoystickPort = event.JoystickEvent.Axis[portJoystickAxis]/32768.0;
                    //If previous value is NAN, store current value in previous and current, otherwise only in current
                    if (previousJoystickPort==INFINITY) {
                        previousJoystickPort = newJoystickPort;
                    }
                }
                if (thisJoystick == stbdJoystickNo && thisAxis == stbdJoystickAxis) {
                    newJoystickStbd = event.JoystickEvent.Axis[stbdJoystickAxis]/32768.0;
                    //If previous value is NAN, store current value in previous and current, otherwise only in current
                    if (previousJoystickStbd==INFINITY) {
                        previousJoystickStbd = newJoystickStbd;
                    }
                }
                if (thisJoystick == rudderJoystickNo && thisAxis == rudderJoystickAxis) {
                    newJoystickRudder = 30*event.JoystickEvent.Axis[rudderJoystickAxis]/32768.0;
                    //If previous value is NAN, store current value in previous and current, otherwise only in current
                    if (previousJoystickRudder==INFINITY) {
                        previousJoystickRudder = newJoystickRudder;
                    }
                }
            }

            //Do joystick stuff here
            //Todo: track joystick changes, so if not changing, the GUI inputs are used - partially implemented but need to check for jitter etc
            //Todo: Also implement multiplier/offset and joystick map.
            //FIXME: Note that Irrlicht does not have joystick handling on MacOS

            //check if any have changed
            bool joystickChanged = false;
            f32 portChange = fabs(newJoystickPort - previousJoystickPort);
            f32 stbdChange = fabs(newJoystickStbd - previousJoystickStbd);
            f32 rudderChange = fabs(newJoystickRudder - previousJoystickRudder);
            if (portChange > 0.01 || stbdChange > 0.01 || rudderChange > 0.01)
            {
                joystickChanged = true;
            }

            //If any have changed, use all (iff non-infinite)
            if (joystickChanged) {
                if (newJoystickPort<INFINITY) {
                    model->setPortEngine(newJoystickPort);
                    previousJoystickPort=newJoystickPort;
                }

                if (newJoystickStbd<INFINITY) {
                    model->setStbdEngine(newJoystickStbd);
                    previousJoystickStbd=newJoystickStbd;
                }

                if (newJoystickRudder<INFINITY) {
                    model->setRudder(newJoystickRudder);
                    previousJoystickRudder=newJoystickRudder;
                }

            }


        }

        return false;

    }


/*
	s32 MyEventReceiver::GetScrollBarPosSpeed() const
	{
		return scrollBarPosSpeed;
	}

	s32 MyEventReceiver::GetScrollBarPosHeading() const
	{
		return scrollBarPosHeading;
	}
*/
