#include "guis/GuiInputConfig.h"
#include "Window.h"
#include "Log.h"
#include "components/TextComponent.h"
#include "components/OptionListComponent.h"
#include "components/ImageComponent.h"
#include "components/MenuComponent.h"
#include "components/ButtonComponent.h"
#include "Util.h"

static const int inputCount = 20;
static const char* inputName[inputCount] = { "Up", "Down", "Left", "Right", "A", "B", "Start", "Select", "PageUp", "PageDown", "X", "Y", "L2", "R2",
    "LAUp", "LALeft", "RAUp", "RALeft", "L3", "R3" };
static const bool inputSkippable[inputCount+1] = { false, false, false, false, false, false, false, false, true, true, true, true, true, true,
    true, true, true, true, true, true, false }; // last one if for the gamepad model
static const char* inputDispName[inputCount] = { "Hat-UP", "Hat-DOWN", "Hat-LEFT", "Hat-RIGHT", "A / Circle / N64-C / Saturn-B",
    "B / Cross / N64-C / Saturn-A", "START", "SELECT / N64-Z-Trigger", "L1 / Saturn-Z / N64-B", "R1 / Saturn-C / N64-A", "X / Triangle / N64-C / Saturn-Y",
    "Y / Square / N64-C / Saturn-X", "L2 / Saturn-L / N64-L", "R2 / Saturn-R / N64-R",
    "Left Analog-UP", "Left Analog-LEFT", "Right Analog-UP", "Right Analog-LEFT", "L3 (Left Analog Button)", "R3 (Right Analog Button)"  };
static const char* inputIcon[inputCount] = { ":/help/dpad_up.svg", ":/help/dpad_down.svg", ":/help/dpad_left.svg", ":/help/dpad_right.svg", 
                                            ":/help/button_east.svg", ":/help/button_south.svg", ":/help/button_start.svg", ":/help/button_select.svg", 
                                            ":/help/button_l.svg", ":/help/button_r.svg", ":/help/button_north.svg", ":/help/button_west.svg",
                                            ":/help/button_l2.svg", ":/help/button_r2.svg",
                                            ":/help/analog_up.svg", ":/help/analog_left.svg", ":/help/analog_up.svg", ":/help/analog_left.svg",
                                            ":/help/analog.svg", ":/help/analog.svg" };

// this is much more useful and necessary than you can imagine
static const char * gamepadNames[] = { "Generic", "Playstation", "Xbox", "Saturn", "N64", "SNES", "Atari", "NES", "Arcade", "\0" };
                                            
//MasterVolUp and MasterVolDown are also hooked up, but do not appear on this screen.
//If you want, you can manually add them to es_input.cfg.

using namespace Eigen;

#define HOLD_TO_SKIP_MS 1000

GuiInputConfig::GuiInputConfig(Window* window, InputConfig* target, bool reconfigureAll, const std::function<void()>& okCallback) : GuiComponent(window), 
    mBackground(window, ":/frame.png"), mGrid(window, Vector2i(1, 7)), 
    mTargetConfig(target), mHoldingInput(false)
{
    LOG(LogInfo) << "Configuring device " << target->getDeviceId() << " (" << target->getDeviceName() << ").";

    if(reconfigureAll)
        target->clear();

    mConfiguringAll = reconfigureAll;
    mConfiguringRow = mConfiguringAll;

    addChild(&mBackground);
    addChild(&mGrid);

    // 0 is a spacer row
    mGrid.setEntry(std::make_shared<GuiComponent>(mWindow), Vector2i(0, 0), false);

    mTitle = std::make_shared<TextComponent>(mWindow, "CONFIGURING", Font::get(FONT_SIZE_LARGE), 0x555555FF, ALIGN_CENTER);
    mGrid.setEntry(mTitle, Vector2i(0, 1), false, true);
    
    std::stringstream ss;
    if(target->getDeviceId() == DEVICE_KEYBOARD)
        ss << "KEYBOARD";
    else
        ss << "GAMEPAD " << (target->getDeviceId() + 1);
    mSubtitle1 = std::make_shared<TextComponent>(mWindow, strToUpper(ss.str()), Font::get(FONT_SIZE_MEDIUM), 0x555555FF, ALIGN_CENTER);
    mGrid.setEntry(mSubtitle1, Vector2i(0, 2), false, true);

    mSubtitle2 = std::make_shared<TextComponent>(mWindow, "PRESS UP/DOWN OR HOLD ANY BUTTON TO SKIP", Font::get(FONT_SIZE_SMALL), 0x99999900, ALIGN_CENTER);
    mGrid.setEntry(mSubtitle2, Vector2i(0, 3), false, true);

    // 4 is a spacer row

    mList = std::make_shared<ComponentList>(mWindow);
    mGrid.setEntry(mList, Vector2i(0, 5), true, true);
    for(int i = 0; i < inputCount; i++)
    {
        ComponentListRow row;
        
        // icon
        auto icon = std::make_shared<ImageComponent>(mWindow);
        icon->setImage(inputIcon[i]);
        icon->setColorShift(0x777777FF);
        icon->setResize(0, Font::get(FONT_SIZE_MEDIUM)->getLetterHeight() * 1.25f);
        row.addElement(icon, false);

        // spacer between icon and text
        auto spacer = std::make_shared<GuiComponent>(mWindow);
        spacer->setSize(16, 0);
        row.addElement(spacer, false);

        auto text = std::make_shared<TextComponent>(mWindow, inputDispName[i], Font::get(FONT_SIZE_MEDIUM), 0x777777FF);
        row.addElement(text, true);

        auto mapping = std::make_shared<TextComponent>(mWindow, "-NOT DEFINED-", Font::get(FONT_SIZE_MEDIUM, FONT_PATH_LIGHT), 0x999999FF, ALIGN_RIGHT);
        setNotDefined(mapping); // overrides text and color set above
        row.addElement(mapping, true);
        mMappings.push_back(mapping);

        row.input_handler = [this, i, mapping](InputConfig* config, Input input) -> bool
        {
            // ignore input not from our target device
            if(config != mTargetConfig)
                return false;

            if(mConfiguringAll && inputSkippable[i] && (config->isMappedTo("up", input) || config->isMappedTo("down", input))) {
                setNotDefined(mapping);
                mConfiguringAll = false;
                mConfiguringRow = false;
                return false;
            }

            // if we're not configuring, start configuring when A is pressed
            if(!mConfiguringRow)
            {
                if(config->isMappedTo("a", input) && input.value)
                {
                    mList->stopScrolling();
                    mConfiguringRow = true;
                    setPress(mapping);
                    return true;
                }
                
                // we're not configuring and they didn't press A to start, so ignore this
                return false;
            }

            // we are configuring
            if(input.value != 0)
            {
                // input down
                // if we're already holding something, ignore this, otherwise plan to map this input
                if(mHoldingInput)
                    return true;

                mHoldingInput = true;
                mHeldInput = input;
                mHeldTime = 0;
                mHeldInputId = i;

                return true;
            }else{
                // input up
                // make sure we were holding something and we let go of what we were previously holding
                if(!mHoldingInput || mHeldInput.device != input.device || mHeldInput.id != input.id || mHeldInput.type != input.type)
                    return true;

                mHoldingInput = false;

                if(assign(mHeldInput, i))
                    rowDone(); // if successful, move cursor/stop configuring - if not, we'll just try again

                return true;
            }
        };

        mList->addRow(row);
    }

    auto gamepad_model = std::make_shared< OptionListComponent<std::string> >(mWindow, "Gamepad Model", false);
    for(const char ** gname = gamepadNames; **gname != '\0'; gname++) {
        std::string gamepad = *gname;
        gamepad_model->add(gamepad, gamepad, gamepad == "Generic");
    }
    ComponentListRow row;
    row.addElement(std::make_shared<TextComponent>(mWindow, "Gamepad Model", Font::get(FONT_SIZE_MEDIUM), 0x777777FF), true);
    row.addElement(gamepad_model, false);
    mList->addRow(row);
    
    // only show "HOLD TO SKIP" if this input is skippable
    mList->setCursorChangedCallback([this](CursorState state) {
        bool skippable = inputSkippable[mList->getCursorId()];
        mSubtitle2->setOpacity(skippable && mConfiguringAll ? 255 : 0);
    });

    // make the first one say "PRESS ANYTHING" if we're re-configuring everything
    if(mConfiguringAll)
        setPress(mMappings.front());

    // buttons
    std::vector< std::shared_ptr<ButtonComponent> > buttons;
    buttons.push_back(std::make_shared<ButtonComponent>(mWindow, "OK", "ok", [this, okCallback, gamepad_model] { 
        mTargetConfig->setDeviceModel(gamepad_model->getSelected());
        InputManager::getInstance()->writeDeviceConfig(mTargetConfig); // save
        if(okCallback)
            okCallback();
        delete this; 
    }));
    mButtonGrid = makeButtonGrid(mWindow, buttons);
    mGrid.setEntry(mButtonGrid, Vector2i(0, 6), true, false);

    setSize(Renderer::getScreenWidth() * 0.6f, Renderer::getScreenHeight() * 0.75f);
    setPosition((Renderer::getScreenWidth() - mSize.x()) / 2, (Renderer::getScreenHeight() - mSize.y()) / 2);
}

void GuiInputConfig::onSizeChanged()
{
    mBackground.fitTo(mSize, Vector3f::Zero(), Vector2f(-32, -32));

    // update grid
    mGrid.setSize(mSize);

    //mGrid.setRowHeightPerc(0, 0.025f);
    mGrid.setRowHeightPerc(1, mTitle->getFont()->getHeight()*0.75f / mSize.y());
    mGrid.setRowHeightPerc(2, mSubtitle1->getFont()->getHeight() / mSize.y());
    mGrid.setRowHeightPerc(3, mSubtitle2->getFont()->getHeight() / mSize.y());
    //mGrid.setRowHeightPerc(4, 0.03f);
    mGrid.setRowHeightPerc(5, (mList->getRowHeight(0) * 5 + 2) / mSize.y());
    mGrid.setRowHeightPerc(6, mButtonGrid->getSize().y() / mSize.y());
}

void GuiInputConfig::update(int deltaTime)
{
    if(mConfiguringRow && mHoldingInput && inputSkippable[mHeldInputId])
    {
        int prevSec = mHeldTime / 1000;
        mHeldTime += deltaTime;
        int curSec = mHeldTime / 1000;

        if(mHeldTime >= HOLD_TO_SKIP_MS)
        {
            setNotDefined(mMappings.at(mHeldInputId));
            clearAssignment(mHeldInputId);
            mHoldingInput = false;
            rowDone();
        }else{
            if(prevSec != curSec)
            {
                // crossed the second boundary, update text
                const auto& text = mMappings.at(mHeldInputId);
                std::stringstream ss;
                ss << "HOLD FOR " << HOLD_TO_SKIP_MS/1000 - curSec << "S TO SKIP";
                text->setText(ss.str());
                text->setColor(0x777777FF);
            }
        }
    }
}

// move cursor to the next thing if we're configuring all, 
// or come out of "configure mode" if we were only configuring one row
void GuiInputConfig::rowDone()
{
    if(mConfiguringAll) {
        // try to move to the next one or reach last line (Gamepad Model)
        if(!mList->moveCursor(1) || mList->getCursorId() == inputCount)
        {
            mConfiguringAll = false;
            mConfiguringRow = false;
//             mGrid.moveCursor(Vector2i(0, 1));
        }else{
            setPress(mMappings.at(mList->getCursorId()));
    }
    }else{
        // only configuring one row, so stop
        mConfiguringRow = false;
    }
}

void GuiInputConfig::setPress(const std::shared_ptr<TextComponent>& text)
{
    text->setText("PRESS ANYTHING");
    text->setColor(0x656565FF);
}

void GuiInputConfig::setNotDefined(const std::shared_ptr<TextComponent>& text)
{
    text->setText("-NOT DEFINED-");
    text->setColor(0x999999FF);
}

void GuiInputConfig::setAssignedTo(const std::shared_ptr<TextComponent>& text, Input input)
{
    text->setText(strToUpper(input.string()));
    text->setColor(0x777777FF);
}

void GuiInputConfig::error(const std::shared_ptr<TextComponent>& text, const std::string& msg, int inputId)
{
    text->setText(inputSkippable[inputId] ? "ALREADY TAKEN. HOLD TO JUMP." : "ALREADY TAKEN");
    text->setColor(0x656565FF);
}

bool GuiInputConfig::assign(Input input, int inputId)
{
    // input is from InputConfig* mTargetConfig

    // if this input is mapped to something other than "nothing" or the current row, error
    // (if it's the same as what it was before, allow it)
    if(mTargetConfig->getMappedTo(input).size() > 0 && !mTargetConfig->isMappedTo(inputName[inputId], input))
    {
        error(mMappings.at(inputId), "Already mapped!", inputId);
        return false;
    }

    setAssignedTo(mMappings.at(inputId), input);
    
    input.configured = true;
    mTargetConfig->mapInput(inputName[inputId], input);

    LOG(LogInfo) << "  Mapping [" << input.string() << "] -> " << inputName[inputId];

    return true;
}

void GuiInputConfig::clearAssignment(int inputId)
{
    mTargetConfig->unmapInput(inputName[inputId]);
}
