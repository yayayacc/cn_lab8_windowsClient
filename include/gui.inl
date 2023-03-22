void GUI::drawGUI() {
    ImGui::Begin("TEST00");
    if (ImGui::Button("Button")) {
        std::cout << "hello world\n";
    }
    ImGui::End();

    ImGui::Begin("TEST01");
    std::string test;
    char        buffer[256];
    memset(buffer, 0, sizeof(buffer));
    strcpy_s(buffer, sizeof(buffer), test.c_str());
    if (ImGui::InputText("Name", buffer, sizeof(buffer))) {
        if (!buffer[0]) buffer[0] = ' ';
        test = std::string(buffer);
    }
    ImGui::End();
}
