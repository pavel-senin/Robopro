#include "mainwindow.h"
#include "./ui_mainwindow.h"



MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Настраиваем tableResult
    // Создаем одну строку
    ui->tableResult->setRowCount(1);
    // Инициализация нулевыми значениями
    for (int col = 0; col < 3; ++col) {
        QTableWidgetItem *item = new QTableWidgetItem("0.00000000");
        item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        ui->tableResult->setItem(0, col, item);
    }
    ui->tableResult->verticalHeader()->setVisible(false);

    connect(ui->computeResult,
            &QPushButton::clicked,
            this,
            [this]() {MainWindow::computeResult();});

    // Настраиваем tableJoints
    ui->tableJoints->verticalHeader()->setVisible(false);

    connect(ui->addJoint,
            &QPushButton::clicked,
            this,
            [this]() {MainWindow::addJoint();});

    connect(ui->deleteJoint,
            &QPushButton::clicked,
            this,
            [this]() {MainWindow::deleteJoint();});

    connect(ui->infoButton,
            &QPushButton::clicked,
            this,
            [this]() {MainWindow::info();});



}

MainWindow::~MainWindow()
{
    delete ui;
}

// Info
void MainWindow::info(){
    QString infoText =
                        "Правила заполнения и использования:<br><br>"
                        "1. Введите все параметры сочленений в соответствующие поля.<br>"
                        "2. <b>Add joint</b> - добавить сочленение<br>"
                        "   <b>Delete joint</b> - удалить сочленение<br>"
                        "   Количество сочленений ограничено в диапазоне от 0 до 9<br>"
                        "3. Поля <b>theta</b>, <b>a</b>, <b>d</b>, <b>alpha</b> должны содержать числовые значения, "
                        "допустимо использование математических выражений и математической константы <i>pi</i>.<br>"
                        "4. Ограничения на границы диапазона каждого значения:<br> "
                        "<b>theta</b> - [-180; 180](deg); <b>a</b> - [-10; 10](m); <b>d</b> - [-10; 10](m); <b>alpha</b> [-pi; pi](rad).<br>"
                        "5. Ввод дробных значений возможен до 6 знаков после запятой.<br>"
                        "6. Для расчета результата нажмите кнопку <b>Compute result</b>.<br>"
                        "7. Если есть незаполненные поля, программа предупредит вас перед расчетом.<br><br>"
                         "Убедитесь, что все данные введены корректно, иначе расчет будет невозможен.";
        QMessageBox::information(this, "Информация", infoText);
}

// Валидация ячеек
void MainWindow::validateCellInput(QLineEdit *lineEdit, int row, int col){
    QString text = lineEdit->text().trimmed();
    text.replace("pi", QString::number(M_PI, 'f', 8));

    QJSEngine engine;
    QJSValue result = engine.evaluate(text);

    if (result.isError()) {
        QMessageBox::warning(this, "Ошибка ввода", "Ошибка в выражении: " + result.toString());
        lineEdit->clear();
        return;
    }

    double value = result.toNumber();
    bool correctValue = false;

    // Проверка на корректность по столбцам
    switch (col) {
    case 1:  // theta (deg): [-180; 180]
        if (value < -180.0 || value > 180.0) {
            QMessageBox::warning(this, "Ошибка ввода", "Значение θ должно быть в диапазоне [-180; 180] градусов.");
            lineEdit->clear();
        } else{
            correctValue = true;
        }
        break;
    case 2:  // a (m): [-1; 1]
        if (value < -10 || value > 10) {
            QMessageBox::warning(this, "Ошибка ввода", "Значение a должно быть в диапазоне [-10; 10] метров.");
            lineEdit->clear();
        } else{
            correctValue = true;
        }
        break;
    case 3:  // d (m): [-1; 1]
        if (value < -10 || value > 10) {  // Задаем подходящий диапазон для смещений
            QMessageBox::warning(this, "Ошибка ввода", "Значение d должно быть в диапазоне [-10; 10] метров.");
            lineEdit->clear();
        } else{
            correctValue = true;
        }
        break;
    case 4:  // alpha (rad): [-pi; pi]
        if (value < -M_PI || value > M_PI) {
            QMessageBox::warning(this, "Ошибка ввода", "Значение α должно быть в диапазоне [-π; π] радиан.");
            lineEdit->clear();
        } else{
            correctValue = true;
        }
        break;
    default:
        break;
    }
    // Устанавливаем значение, если корректно
    if (correctValue){
        lineEdit->setText(QString::number(value, 'f', 6));
    }
}



// Добавить новую строку/Joint
void MainWindow::addJoint(){
    int currentRow = ui->tableJoints->rowCount();

    // Ограничим максимальное количество Joints == 10
    if (currentRow >= 10){
        QMessageBox::warning(this, "Warning", "Количество joints ограничено, больше нельзя создавать joints");
        return;
    }
    // Устанавливаем номер Joint в первом столбце
    ui->tableJoints->insertRow(currentRow);
    QTableWidgetItem *jointItem = new QTableWidgetItem(QString::number(currentRow));
    jointItem->setFlags(Qt::ItemIsEnabled);
    ui->tableJoints->setItem(currentRow, 0, jointItem);
    // Валидатор для чисел до 6 знаков после запятой и поддержка "pi"
    QRegularExpression rx(R"(^$|(pi)|(-?\d+(\.\d{0,6})?))");
    QRegularExpressionValidator validator(rx, this);
    // Добавляем пустые редактируемые ячейки
    for (int col = 0; col < ui->tableJoints->columnCount(); ++col) {
        QTableWidgetItem* item = new QTableWidgetItem();
        item->setFlags(Qt::ItemIsEditable | Qt::ItemIsEnabled);
        ui->tableJoints->setItem(ui->tableJoints->rowCount(), col, item);

        if (col >= 1){
            QLineEdit *lineEdit = new QLineEdit();
            lineEdit->setValidator(&validator);
            connect(lineEdit,
                    &QLineEdit::editingFinished,
                    this,
                    [=]() {MainWindow::validateCellInput(lineEdit,currentRow, col);});
            ui->tableJoints->setCellWidget(currentRow, col, lineEdit);
        }
    }
}

// Удалить строку/Joint
void MainWindow::deleteJoint(){
    // Получаем индекс текущей выбранной строки
    int currentRow = ui->tableJoints->currentRow();

    // Проверяем, выбрана ли строка (если нет, currentRow будет == -1)
    if (currentRow >= 0) {
        ui->tableJoints->removeRow(currentRow);
        // После удаления строки обновляем нумерацию Joint
        for (int row = 0; row < ui->tableJoints->rowCount(); ++row) {
            QTableWidgetItem *jointItem = ui->tableJoints->item(row, 0);
            jointItem->setText(QString::number(row + 1));
        }
    } else {
        QMessageBox::warning(this, "Ошибка", "Выберите строку для удаления.");
    }
}

// Расчет результата
void MainWindow::computeResult(){
    // Сбор данных из таблицы по столбцам
    std::vector<double> thetas;
    std::vector<double> as;
    std::vector<double> ds;
    std::vector<double> alphas;
    if (ui->tableJoints->rowCount() == 0){
        QMessageBox::warning(this, "Warning", "Отсутствуют соединения для расчета.");
        return;
    }
    for (size_t i = 0; i < ui->tableJoints->rowCount(); ++i){
        // Собираем содержимое ячеек
        QLineEdit *thetaEdit = qobject_cast<QLineEdit *>(ui->tableJoints->cellWidget(i, 1));
        QLineEdit *aEdit = qobject_cast<QLineEdit *>(ui->tableJoints->cellWidget(i, 2));
        QLineEdit *dEdit = qobject_cast<QLineEdit *>(ui->tableJoints->cellWidget(i, 3));
        QLineEdit *alphaEdit = qobject_cast<QLineEdit *>(ui->tableJoints->cellWidget(i, 4));
        // Существование ячеек
        if (!thetaEdit ||
            !aEdit ||
            !dEdit ||
            !alphaEdit) {
            QMessageBox::warning(this, "Warning", "Некоторые ячейки отсутствуют.");
            return;
        }
        // Проверка на пустое значение в ячейке
        if (thetaEdit->text().isEmpty() ||
            aEdit->text().isEmpty() ||
            dEdit->text().isEmpty() ||
            alphaEdit->text().isEmpty()) {
            QMessageBox::warning(this, "Warning", "Все ячейки должны быть заполнены.");
            return;
        }
        thetas.push_back(thetaEdit->text().toDouble());
        as.push_back(aEdit->text().toDouble());
        ds.push_back(dEdit->text().toDouble());
        alphas.push_back(alphaEdit->text().toDouble());
    }

    // Расчет dh-модели
    dh_model dhBot(thetas, as, ds, alphas);

    // Результат кинематики (координаты x, y, z)
    std::vector<double> result = dhBot.calculateEndPosition();

    // Округление до 8 знаков после запятой
    for (int i = 0; i < 3; ++i) {
        double roundedValue = std::round(result[i] * 100000000.0) / 100000000.0;
        // Обновляем ячейку таблицы
        QTableWidgetItem *item = ui->tableResult->item(0, i);
        if (!item) {
            item = new QTableWidgetItem();
            ui->tableResult->setItem(0, i, item);
        }
        item->setText(QString::number(roundedValue, 'f', 8));
    }
}


