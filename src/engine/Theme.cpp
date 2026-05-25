#include "engine/Theme.h"

namespace ronin::engine {

static ImVec4 toIm(int r, int g, int b, int a = 255) {
    return ImVec4(r/255.f, g/255.f, b/255.f, a/255.f);
}

void applyImGuiTheme(ThemeId id) {
    ImGuiStyle& s = ImGui::GetStyle();
    s.WindowRounding = 6.f;
    s.FrameRounding = 4.f;
    s.GrabRounding = 4.f;
    s.WindowPadding = ImVec2(10, 10);
    s.ItemSpacing = ImVec2(8, 6);
    auto& c = s.Colors;
    ImVec4 bg, panel, accent, accentHov, text;
    switch (id) {
        case ThemeId::SamuraiNeon:
            bg=toIm(12,12,18,240); panel=toIm(24,18,30,240);
            accent=toIm(220,30,80); accentHov=toIm(255,80,130);
            text=toIm(240,230,230); break;
        case ThemeId::Matrix:
            bg=toIm(4,16,8,240); panel=toIm(8,28,14,240);
            accent=toIm(40,220,90); accentHov=toIm(120,255,130);
            text=toIm(200,255,210); break;
        case ThemeId::Tron:
            bg=toIm(6,10,20,240); panel=toIm(10,18,32,240);
            accent=toIm(60,200,255); accentHov=toIm(150,235,255);
            text=toIm(220,240,255); break;
        case ThemeId::DarkMinimal:
            bg=toIm(20,20,24,240); panel=toIm(28,28,34,240);
            accent=toIm(180,180,190); accentHov=toIm(230,230,240);
            text=toIm(230,230,235); break;
    }
    c[ImGuiCol_WindowBg]=bg; c[ImGuiCol_ChildBg]=panel; c[ImGuiCol_PopupBg]=panel;
    c[ImGuiCol_FrameBg]=panel; c[ImGuiCol_FrameBgHovered]=accent; c[ImGuiCol_FrameBgActive]=accentHov;
    c[ImGuiCol_TitleBg]=panel; c[ImGuiCol_TitleBgActive]=accent;
    c[ImGuiCol_Button]=panel; c[ImGuiCol_ButtonHovered]=accent; c[ImGuiCol_ButtonActive]=accentHov;
    c[ImGuiCol_Header]=panel; c[ImGuiCol_HeaderHovered]=accent; c[ImGuiCol_HeaderActive]=accentHov;
    c[ImGuiCol_Tab]=panel; c[ImGuiCol_TabHovered]=accent; c[ImGuiCol_TabActive]=accentHov;
    c[ImGuiCol_CheckMark]=accentHov; c[ImGuiCol_SliderGrab]=accent; c[ImGuiCol_SliderGrabActive]=accentHov;
    c[ImGuiCol_Text]=text; c[ImGuiCol_Border]=accent;
}

} // namespace ronin::engine
