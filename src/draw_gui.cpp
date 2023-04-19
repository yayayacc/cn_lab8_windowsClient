#include "gui.h"

static void drawInputText(std::string& str, const std::string& label) {
    char buffer[256];
    memset(buffer, 0, sizeof(buffer));
    strcpy_s(buffer, sizeof(buffer), str.c_str());
    if (ImGui::InputText(label.c_str(), buffer, sizeof(buffer))) {
        if (!buffer[0]) buffer[0] = ' ';
        str = std::string(buffer);
    }
}

static void drawMTLInputText(std::string& str, const std::string& label) {
    char buffer[256];
    memset(buffer, 0, sizeof(buffer));
    strcpy_s(buffer, sizeof(buffer), str.c_str());
    if (ImGui::InputTextMultiline(label.c_str(), buffer, sizeof(buffer))) {
        if (!buffer[0]) buffer[0] = ' ';
        str = std::string(buffer);
    }
}

void GUI::drawGUI() {
    switch (state) {
        case STATE::NOT_LOG_IN: {
            loginGUI();
        } break;
        case STATE::HAS_LOG_IN: {
            commonGUI();
        } break;
        default:
            break;
    }
}

void GUI::loginGUI() {
    static std::string account{"cc12345678"};
    static std::string pwd{"123456"};
    static bool        is_warning{false};

    ImGui::Begin("LOG_IN");
    drawInputText(account, "account");
    drawInputText(pwd, "pwd");

    if (is_warning)
        ImGui::TextColored(ImVec4{1.0, 0.0, 0.0, 1.0}, "Account or pwd is incorrect!!!");

    if (ImGui::Button("LOG_IN")) {
        if (client.logIn(account, pwd) == 1)
            state = STATE::HAS_LOG_IN;
        else
            is_warning = true;
    }
    ImGui::End();
}

void GUI::commonGUI() {
    drawList();
    drawInput();
    drawText();
}

void GUI::drawList() {
    ImGui::Begin("List");
    {
        if (ImGui::Selectable("cc12345678", app_data.selected_item == "cc12345678")) {
            app_data.selected_item  = "cc12345678";
            app_data.send_msg_item  = "cc123456RM";
            app_data.send_file_item = "cc123456RF";
            app_data.is_group       = false;
        }

        if (ImGui::Selectable("core123456", app_data.selected_item == "core123456")) {
            app_data.selected_item  = "core123456";
            app_data.send_msg_item  = "core1234RM";
            app_data.send_file_item = "core1234RF";
            app_data.is_group       = false;
        }

        if (ImGui::Selectable("godlike123", app_data.selected_item == "godlike123")) {
            app_data.selected_item  = "godlike123";
            app_data.send_msg_item  = "godlike_RM";
            app_data.send_file_item = "godlike_RF";
            app_data.is_group       = false;
        }

        if (ImGui::Selectable("groupChat1", app_data.selected_item == "groupChat1")) {
            app_data.selected_item  = "groupChat1";
            app_data.send_msg_item  = "groupChat1";
            app_data.send_file_item = "groupChat1";
            app_data.is_group       = true;
        }
    }
    ImGui::End();
}

static std::string openFile(const char* filter = "") {
    OPENFILENAMEA ofn;
    CHAR          szFile[260] = {0};
    ZeroMemory(&ofn, sizeof(OPENFILENAME));
    ofn.lStructSize  = sizeof(OPENFILENAME);
    ofn.hwndOwner    = FindWindow(L"MainWnd", NULL);
    ofn.lpstrFile    = szFile;
    ofn.nMaxFile     = sizeof(szFile);
    ofn.lpstrFilter  = filter;
    ofn.nFilterIndex = 1;
    ofn.Flags        = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;
    if (GetOpenFileNameA(&ofn) == TRUE) {
        return ofn.lpstrFile;
    }
    return std::string{};
}

void GUI::drawInput() {
    ImGui::Begin("Input");
    drawMTLInputText(app_data.input_buffer, "##Message");

    if (ImGui::Button("Send")) {
        if (!app_data.is_group)
            client.Msg2User(app_data.send_msg_item, app_data.input_buffer);
        else
            client.Msg2Group(app_data.send_msg_item, client.myName + ":" + app_data.input_buffer);

        app_data.msg_record[app_data.selected_item].append("Me:" + app_data.input_buffer + "\n");

        app_data.input_buffer.clear();
    }

    ImGui::End();

    ImGui::Begin("File");

    if (ImGui::Button("Send")) {
        client.transferFile(app_data.send_file_item, openFile());
    }

    ImGui::End();
}

void GUI::drawText() {
    ImGui::Begin("Text");

    ImGui::Text("%s", app_data.msg_record[app_data.selected_item].c_str());

    ImGui::End();
}
