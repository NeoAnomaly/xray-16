#include "stdafx.h"
#include "UIChangeWeather.h"
#include "UIXmlInit.h"
#include "UI3tButton.h"
#include "../game_cl_teamdeathmatch.h"
#include "UIKickPlayer.h"
#include "UIChangeMap.h"
#include <dinput.h>
#include "xrEngine/XR_IOConsole.h"
#include "UIMapList.h"
#include "../UIGameCustom.h"

ButtonListDialog::ButtonListDialog()
{
    Background = xr_new<CUIStatic>();
    Background->SetAutoDelete(true);
    AttachChild(Background);
    Header = xr_new<CUITextWnd>();
    Header->SetAutoDelete(true);
    AttachChild(Header);
    CancelButton = xr_new<CUI3tButton>();
    CancelButton->SetAutoDelete(true);
    AttachChild(CancelButton);    
}

void ButtonListDialog::Initialize(int buttonCount)
{
    buttons.reserve(buttonCount);
    for (int i = 0; i < buttonCount; i++)
    {
        NamedButton btn;
        btn.Button = xr_new<CUI3tButton>();
        btn.Button->SetAutoDelete(true);
        AttachChild(btn.Button);
        btn.Text = xr_new<CUITextWnd>();
        btn.Text->SetAutoDelete(true);
        AttachChild(btn.Text);
        buttons.push_back(btn);
    }
}

void ButtonListDialog::OnCancel()
{
    HideDialog();
}

const ButtonListDialog::NamedButton& ButtonListDialog::GetButton(int i) const
{
    return buttons[i];
}

bool ButtonListDialog::OnKeyboardAction(int dik, EUIMessages keyboardAction)
{
    CUIDialogWnd::OnKeyboardAction(dik, keyboardAction);
    if (WINDOW_KEY_PRESSED == keyboardAction)
    {
        if (DIK_ESCAPE == dik)
        {
            OnCancel();
            return true;
        }
        int btnCount = buttons.size();
        if (dik >= DIK_1 && dik <= DIK_1-1+btnCount && btnCount <= 9) // handle 1..9 keys only
        {
            OnButtonClick(dik-DIK_1);
            return true;
        }
    }
    return false;
}

void ButtonListDialog::SendMessage(CUIWindow* wnd, s16 msg, void* data /*= nullptr */)
{
    if (BUTTON_CLICKED == msg)
    {
        if (CancelButton == wnd)
            OnCancel();
        for (u32 i = 0; i < buttons.size(); i++)
        {
            if (buttons[i].Button == wnd)
            {
                OnButtonClick(i);
                return;
            }
        }
    }
}

void CUIChangeWeather::InitChangeWeather(CUIXml& xmlDoc)
{
    CUIXmlInit::InitWindow(xmlDoc, "change_weather", 0, this);
    CUIXmlInit::InitTextWnd(xmlDoc, "change_weather:header", 0, Header);
    CUIXmlInit::InitStatic(xmlDoc, "change_weather:background", 0, Background);
    CUIXmlInit::Init3tButton(xmlDoc, "change_weather:btn_cancel", 0, CancelButton);
    auto& gameWeathers = gMapListHelper.GetGameWeathers();
    Initialize(gameWeathers.size());
    weatherItems.resize(gameWeathers.size());
	string256 path;
    for (u32 i = 0; i < weatherItems.size(); i++)
    {
        xr_sprintf(path, "change_weather:btn_%d", i+1);
        CUIXmlInit::Init3tButton(xmlDoc, path, 0, GetButton(i).Button);
        xr_sprintf(path, "change_weather:txt_%d", i+1);
        CUIXmlInit::InitTextWnd(xmlDoc, path, 0, GetButton(i).Text);
        weatherItems[i].Name = gameWeathers[i].m_weather_name;
        weatherItems[i].Time = gameWeathers[i].m_start_time;
    }
}

void CUIChangeWeather::OnButtonClick(int i)
{
    string1024 command;
    xr_sprintf(command, "cl_votestart changeweather %s %s", *weatherItems[i].Name, *weatherItems[i].Time);
    Console->Execute(command);
    HideDialog();
}

void CUIChangeGameType::InitChangeGameType(CUIXml& xmlDoc)
{
    CUIXmlInit::InitWindow(xmlDoc, "change_gametype", 0, this);
    CUIXmlInit::InitTextWnd(xmlDoc, "change_gametype:header", 0, Header);
    CUIXmlInit::InitStatic(xmlDoc, "change_gametype:background", 0, Background);
    CUIXmlInit::Init3tButton(xmlDoc, "change_gametype:btn_cancel", 0, CancelButton);
    // XXX nitrocaster: get it from somewhere
    const int gameTypeCount = 4;
    Initialize(gameTypeCount);
    gameTypeItems.resize(gameTypeCount);
    string256 path;
    for (u32 i = 0; i < gameTypeItems.size(); i++)
    {
        xr_sprintf(path, "change_gametype:btn_%d", i+1);
        CUIXmlInit::Init3tButton(xmlDoc, path, 0, GetButton(i).Button);
        xr_sprintf(path, "change_gametype:txt_%d", i+1);
        CUIXmlInit::InitTextWnd(xmlDoc, path, 0, GetButton(i).Text);
        gameTypeItems[i] = xmlDoc.ReadAttrib(path, 0, "id");
    }
}

void CUIChangeGameType::OnButtonClick(int i)
{
    string1024 command;
    xr_sprintf(command, "cl_votestart changegametype %s", gameTypeItems[i].c_str());
    Console->Execute(command);
    HideDialog();
}
