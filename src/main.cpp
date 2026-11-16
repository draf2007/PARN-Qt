#include <QApplication>   // класс «приложение»: цикл событий, обработка мыши и клавиатуры
#include <QMainWindow>    // класс «главное окно»

int main(int argc, char *argv[])   // отсюда начинается любая программа на C++
{
    QApplication app(argc, argv);  // создаём приложение (ровно одно на программу)

    QMainWindow window;            // создаём окно (пока пустое)
    window.setWindowTitle("PARN Setup");
    window.resize(800, 600);
    window.show();                 // показываем окно

    return app.exec();             // запускаем цикл событий; выходим, когда окно закроют
}