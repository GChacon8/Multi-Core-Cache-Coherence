#include <iostream>
#include <bitset>
#include <cstdint>
#include <fstream>
#include <thread>
#include <vector>
#include <unistd.h>
#include <QtWidgets/QApplication>
#include <QtWidgets/QWidget>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QCheckBox>
#include <QtCore/QTimer>
#include <QDebug>
#include "PE.cpp"
#include "Ram.h"
#include "ROM.h"
#include "BusInterconnect.h"

// Clase para la ventana principal
class MainWindow : public QWidget {
private:
    QTableWidget *table;
    QTableWidget *table2;
    QTableWidget *table3;
    QLabel *welcomeLabel, *label0, *label1, *label2, *label3, *label4, *label5, *label6;
    QCheckBox *stepperCheckBox;
    QPushButton *nextButton;
    QPushButton *buttonSteps;

    bool stepper;
    int numPEs;
    Ram ram;
    std::vector<Cache *> caches;
    BusInterconnect *bus;
    PE *cores[2];
    std::thread bus_thread;
    std::thread sim_thread;
    std::thread updateThread;
    

public:
    MainWindow(QWidget *parent = nullptr) : QWidget(parent), numPEs(2), ram() {
        // Inicializar componentes UI
        initUI();

        // Inicializar RAM
        for (int i = 0; i < 256; ++i) {
            ram.write_mem(i, 0);
        }

        stepper = false;

        // Configurar bus y caches
        bus = new BusInterconnect(ram, numPEs, caches);

        // Inicializar PEs
        /*
        for (int i = 0; i < numPEs; i++){
            cores[i] = new PE(i, "/home/dcastroe/Desktop/Arqui2/Multi-Core-Cache-Coherence/ROMPE1.txt", *bus);
            caches.push_back(cores[i]->get_cache());
        }
        */

        cores[0] = new PE(0, "/home/dcastroe/Desktop/Arqui2/Multi-Core-Cache-Coherence/ROMPE1.txt", *bus);
        cores[1] = new PE(1, "/home/dcastroe/Desktop/Arqui2/Multi-Core-Cache-Coherence/ROMPE0.txt", *bus);
        //cores[2] = new PE(2, "/home/dcastroe/Desktop/Arqui2/Multi-Core-Cache-Coherence/ROMPE0.txt", *bus);
        //cores[3] = new PE(3, "/home/dcastroe/Desktop/Arqui2/Multi-Core-Cache-Coherence/ROMPE0.txt", *bus);
        
        caches.push_back(cores[0]->get_cache());
        caches.push_back(cores[1]->get_cache());
        //caches.push_back(cores[2]->get_cache());
        //caches.push_back(cores[3]->get_cache());

    }

    ~MainWindow() {
        delete bus;
        for (int i = 0; i < 2; ++i) {
            delete cores[i];
        }
    }

    void initUI() {
        // Crear etiquetas
        welcomeLabel = new QLabel("Welcome to the MESI Simulator", this);
        label0 = new QLabel("Made By:", this);
        label1 = new QLabel("- Daniel Casto Elizondo", this);
        label2 = new QLabel("- Gabriel Chacon Alfaro", this);
        label3 = new QLabel("- Jose Eduardo Cruz Vergas", this);
        label4 = new QLabel("- Marco Vinicio Rivera Serrano", this);
        label5 = new QLabel("The simulator consists of visualizing the steps or executing directly from a ROM file.", this);
        label6 = new QLabel("It uses 4 processing elements.", this);

        //First Table
        table = new QTableWidget(numPEs, 4);
        table->setHorizontalHeaderLabels(QStringList() << "Core" << "Cache Misses" << "Cache Hits" << "Data Transmitted");
        table->setEditTriggers(QAbstractItemView::NoEditTriggers);
        connect(table, &QTableWidget::cellChanged, table, &QTableWidget::resizeColumnsToContents);
        table->hide();

        //Second Table
        table2 = new QTableWidget(1, 6);
        table2->setHorizontalHeaderLabels(QStringList() << " " << "Invalidations" << "Read Requests" << "Read Responses" << "Write Requests" << "Write Responses");
        table2->setEditTriggers(QAbstractItemView::NoEditTriggers);
        connect(table2, &QTableWidget::cellChanged, table2, &QTableWidget::resizeColumnsToContents);
        table2->hide();

          // Inicialización original de table3
        table3 = new QTableWidget(numPEs, 4);
        table3->setHorizontalHeaderLabels(QStringList() << "Cache" << "Address" << "Old State" << "New State");
        table3->setEditTriggers(QAbstractItemView::NoEditTriggers);
        connect(table3, &QTableWidget::cellChanged, table3, &QTableWidget::resizeColumnsToContents);
        table3->hide();

        // Crear checkbox
        stepperCheckBox = new QCheckBox("Do you want to use the Stepper?", this);
        connect(stepperCheckBox, &QCheckBox::stateChanged, this, &MainWindow::updateStepperState);

        // Crear botón
        nextButton = new QPushButton("Next", this);
        connect(nextButton, &QPushButton::clicked, this, &MainWindow::onNextButtonClicked);

        buttonSteps = new QPushButton("Step", this);
        connect(buttonSteps, &QPushButton::clicked, this, &MainWindow::onButtonStepsClicked);
        buttonSteps->hide();

        // Layout
        QVBoxLayout *labelLayout = new QVBoxLayout();
        labelLayout->addWidget(welcomeLabel);
        labelLayout->addWidget(label0);
        labelLayout->addWidget(label1);
        labelLayout->addWidget(label2);
        labelLayout->addWidget(label3);
        labelLayout->addWidget(label4);
        labelLayout->addWidget(label5);
        labelLayout->addWidget(label6);
        labelLayout->setAlignment(Qt::AlignCenter);

        QVBoxLayout *mainLayout = new QVBoxLayout();
        mainLayout->addLayout(labelLayout);
        mainLayout->addWidget(table);
        mainLayout->addWidget(table2);
        mainLayout->addWidget(table3);
        mainLayout->addWidget(stepperCheckBox);
        mainLayout->addWidget(nextButton);
        mainLayout->addWidget(buttonSteps);
        setLayout(mainLayout);
    }

    void updateStepperState(int state) {
        stepper = (state == Qt::Checked);
        qDebug() << "Stepper state actualizado:" << stepper;
    }

    void onNextButtonClicked() {
        welcomeLabel->hide();
        label0->hide();
        label1->hide();
        label2->hide();
        label3->hide();
        label4->hide();
        label5->hide();
        label6->hide();
        stepperCheckBox->hide();
        nextButton->hide();
        table->show();
        table2->show();
        table3->show();
        if(stepper){
            buttonSteps->show();
        }
        updateStats();

        sim_thread = std::thread(&MainWindow::startSimulation, this);
    }

    void onButtonStepsClicked(){
        int ROM_size;
        int counter = 0;
        for (int i = 0; i < numPEs; i++){
            ROM_size = cores[i]->getMyRomSize();
            if(counter < ROM_size){
                cores[i]->next();
                qDebug()<<"Next Step Done";
            }
        }
    
    }

    void startSimulation() {
        qDebug()<<"simulación iniciadaaa";
        bus_thread = std::thread(&BusInterconnect::processRequests, bus);
        //updateThread = std::thread(&MainWindow::updateStats, this);
        

        if (!stepper) {
            std::thread pe_threads[numPEs];
            for (int i = 0; i < numPEs; ++i) {
                pe_threads[i] = std::thread(&MainWindow::pe_Next, this, cores[i]);
            }

            while (!pe_threads[0].joinable() && !pe_threads[1].joinable()){
                continue;
            }
            for (int i = 0; i < numPEs; ++i) {
                pe_threads[i].join();
            }

            bus->stopBus = true;
            bus_thread.join();

            showResults();
            
        } else {
            qDebug() << "Modo stepper Agregar lógica";
        }

    }

    void pe_Next(PE *core) {
        int counter = 0;
        int ROM_size = core->getMyRomSize();
        while (counter <= ROM_size) {
            usleep(100000); // 100 ms
            core->next();
            counter++;
        }
        qDebug() << "Termino el hilo en:" << core->get_id();
    }

    QString mesiStateToString(MESIState state) {
    switch (state) {
        case MODIFIED:  return "Modified";
        case EXCLUSIVE: return "Exclusive";
        case SHARED:    return "Shared";
        case INVALID:   return "Invalid";
        default:        return "Unknown";
    }
}

    //Create The timer for the exe of refreshTable
    void updateStats() {
        QTimer *timer = new QTimer(this); // Crea un temporizador
        connect(timer, &QTimer::timeout, this, &MainWindow::refreshTables);
        timer->start(100);  // 100 ms para actualizar la tabla
    }

    void refreshTables() {
        for (int i = 0; i < numPEs; ++i) {
            table->setItem(i, 0, new QTableWidgetItem("Core" + QString::number(i)));
            table->setItem(i, 1, new QTableWidgetItem(QString::number(cores[i]->get_cache()->get_miss_count())));
            table->setItem(i, 2, new QTableWidgetItem(QString::number(cores[i]->get_cache()->get_hit_count())));
            table->setItem(i, 3, new QTableWidgetItem(QString::number(bus->getDataTransmitted(i)) + "KB"));

            //table->setItem(i, 1, new QTableWidgetItem(QString::number(cores[i]->get_cache().get));
            // table->setItem(i, 2, new QTableWidgetItem(QString::number(cores[i]->getMisses())));
            // table->setItem(i, 3, new QTableWidgetItem(QString::number(bus->getInvalidationsByCore(i))));

            //General Stats Table
            table2->setItem(i, 0, new QTableWidgetItem("General Stats"));
            table2->setItem(i, 1, new QTableWidgetItem(QString::number(bus->getNumInvalidations())));
            table2->setItem(i, 2, new QTableWidgetItem(QString::number(bus->getNumReadRequests())));
            table2->setItem(i, 3, new QTableWidgetItem(QString::number(bus->getNumReadResponses())));
            table2->setItem(i, 4, new QTableWidgetItem(QString::number(bus->getNumWriteRequests())));
            table2->setItem(i, 5, new QTableWidgetItem(QString::number(bus->getNumWriteResponses())));

            //Cache registers
            table3->setItem(i, 0, new QTableWidgetItem("Cache" + QString::number(i)));
            table3->setItem(i, 1, new QTableWidgetItem(QString::number(cores[i]->get_cache()->get_last_addr())));
            table3->setItem(i, 2, new QTableWidgetItem(mesiStateToString(cores[i]->get_cache()->get_last_oldState())));
            table3->setItem(i, 3, new QTableWidgetItem(mesiStateToString(cores[i]->get_cache()->get_last_newState())));


            //table3->setItem(i, 2, new QTableWidgetItem(QString::number(cores[i]->get_cache()->get_last_oldState())));
            //table3->setItem(i, 3, new QTableWidgetItem(QString::number(cores[i]->get_cache()->get_last_newState())));

            
        
        }
    }

    void showResults() {
        qDebug() << "Resultados del bus:";
        qDebug() << "Invalidaciones:" << bus->getNumInvalidations();
        // Agrega más estadísticas si es necesario
    }


};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    MainWindow window;
    window.setWindowTitle("MESI Simulator");
    window.setFixedSize(800, 600);
    window.show();
    return app.exec();
}
