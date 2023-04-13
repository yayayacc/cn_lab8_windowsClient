#pragma warning(disable : 4996)

#include "gui.h"

int main() {
    GUI::getInstance().init();
    GUI::getInstance().run();
    GUI::getInstance().shutdown();
}
