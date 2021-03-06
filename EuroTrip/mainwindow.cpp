#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "customplan.h"

#include <QDebug>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("Euro Trip");
    ui->adminFuncs->setVisible(false);
    ui->actionLog_Out->setVisible(false);
    ui->actionLoad_Extended->setVisible(false);

    currentDataFilePath = ":/Files/EuropeanDistancesandFoods.txt";
    ui->comboBoxCities->addItem("Select...");

    ui->cityList->append("Click \"File\" and \"Load\" to start...");
}

MainWindow::~MainWindow()
{
    delete ui;
}
void MainWindow::on_actionExit_triggered()
{
    QApplication::quit();
}

void MainWindow::on_actionAdmin_triggered()
{
    login logWindow;
    connect(&logWindow, &login::userIsAdmin, this, &MainWindow::userIsAdmin);
    logWindow.setModal(true);
    logWindow.exec();
}

void MainWindow::on_actionLog_Out_triggered()
{
    ui->adminFuncs->setVisible(false);
    ui->actionLog_Out->setVisible(false);
    ui->actionLoad_Extended->setVisible(false);
}

void MainWindow::userIsAdmin()
{
    QMessageBox::information(this, "Login", "Username and Password is Correct");
    ui->adminFuncs->setVisible(true);
    ui->actionLog_Out->setVisible(true);
    ui->actionLoad_Extended->setVisible(true);
}

void MainWindow::on_actionLoad_triggered()
{
    ui->cityList->clear();

    currentDataFilePath = ":/Files/EuropeanDistancesandFoods.txt";
    readData(currentDataFilePath);

    // Loads all of current QVector cityListData into Admin

    ui->comboBox_SelectCityAddFood->clear();
    for(int i = 0; i < cityListData.size(); ++i){
        ui->comboBox_SelectCityAddFood->addItem(cityListData[i].getCityName());
    }
}


void MainWindow::on_actionLoad_Extended_triggered()
{
    ui->cityList->clear();
    currentDataFilePath = ":/Files/EuropeanDistancesandFoodsExt.txt";
    readData(currentDataFilePath);

    // Loads all of current QVector cityListData into Admin

    ui->comboBox_SelectCityAddFood->clear();
    for(int i = 0; i < cityListData.size(); ++i){
        ui->comboBox_SelectCityAddFood->addItem(cityListData[i].getCityName());
    }
}

bool MainWindow::alreadyExistingCity(QString checkCity)
{
    City *cityInfo = new City;
    for(int i = 0; i < cityListData.size(); i++)
    {
        *cityInfo = cityListData.value(i);
        if(cityInfo->getCityName() == checkCity)
        {
            return true;
        }
    }
    return false;
}
// Read Data In and Display on UI
void MainWindow::readData(QString dataFile)
{
    QFile file(dataFile);

    if(!file.open(QIODevice::ReadOnly))
    {
        QMessageBox::information(0, "info", file.errorString());
    }
    QTextStream in(&file);

    City *cityInfo;             // city variable used to input cities into the city vector
    bool moreCities= true;
    bool moreFood = true;
    QString cityName;
    QString foodName;
    QString price;
    double  foodPrice;

    double longitude;
    double latitude;

    QString tempLong;
    QString tempLat;

    QString tempLongDir;
    QString tempLatDir;

    QGeoCoordinate berlin;
    berlin.setLatitude(52.5200);
    berlin.setLongitude(13.4050);

    QString berlinDist;


    //reads in the input from the file
    while(moreCities)
    {
        cityInfo = new City;
        //if the line read is NOT a new line character, cityname equals the readline(which is city name) and moves on to the next loop
        //if the line read IS A new line character, the loop exits.
        cityName = in.readLine();
        if(alreadyExistingCity(cityName))
        {
            cityName = in.readLine();
            while(!cityName.isEmpty())
                cityName = in.readLine();
        }
        if((!cityName.isEmpty()))
        {

            cityInfo->setCityName(cityName);

            latitude = in.readLine().toDouble();
            longitude = in.readLine().toDouble();

            cityInfo->setLatitude(latitude);
            cityInfo->setLongitude(longitude);

            tempLatDir = in.readLine();
            tempLongDir = in.readLine();

            cityInfo->setLatDir(tempLatDir);
            cityInfo->setLongDir(tempLongDir);

            //reads in input for the food name and price
            while(moreFood)
            {
                //if the line read is NOT a new line character, foodname equals the readline(which is the food name) and moves on to read in the price.
                //if the line read IS A new line character, the loop exits to read in the next city.
                foodName = in.readLine();
                if(!foodName.isEmpty())
                {
                    foodPrice = in.readLine().toDouble();        //Converts the read in string into a double value
                    cityInfo->addNewFoodItem(foodName, foodPrice);//adds the food name and price into the vector array inside the city's vector array for food and price lol.
                }
                else
                {
                    moreFood = false;
                }
            }
            cityListData.push_back(*cityInfo); //this adds the city object into the vector array
            ui->comboBoxCities->addItem(cityName);
            moreFood = true;
        }
        else
        {
            moreCities = false;
        }
    }

    // read in distance data
    readDistances(cityListData);

    QString temp;

    for(int q = 0; q < cityListData.size(); q++)
    {
        //ui->textBrowser->append(cityListData[q].getCityName());
        for(int r = 0; r < 13; r++)
        {
            temp = QString::number(cityListData[q].getAllDistances()[r]);

           // ui->textBrowser->append(temp);
        }
    }
    // Print data on UI
    for(int loop= 0; loop < cityListData.size(); loop++)//this loops through the main city vecotr to print the info on each city.
    {

       *cityInfo = cityListData.value(loop);
       ui->cityList->append(cityInfo->getCityName()); //this is supposed to display the city name onto the cityList text box on the gui.

       tempLat = QString::number(cityInfo->getLatitude());
       tempLong = QString::number(cityInfo->getLongitude());

       berlinDist = QString::number(cityInfo->getAllDistances()[1]);
       ui->cityList->append("Distance from Berlin: " + berlinDist + "km");

       ui->cityList->append("");
    }
}

void MainWindow::on_buttonGenerate_clicked()
{
    ui->cityInfo->clear();
    QString selectedCity = ui->comboBoxCities->currentText();
    QVector<QPair<QString, double>> foodList;
    for(int i = 0; i < cityListData.size(); i++)
    {
        if(cityListData[i].getCityName() == selectedCity)
        {
            foodList = cityListData[i].getAllFood();
            ui->cityInfo->append(cityListData[i].getCityName() + "\n----------------");
            ui->cityInfo->append("Distance from Berlin: " + QString::number(cityListData[i].getDistance("Berlin")) + "km"
                                 + "\n----------------");
            for(int j = 0; j < foodList.size(); j++)
            ui->cityInfo->append(foodList[j].first + ": $" + QString::number(foodList[j].second));

            i = cityListData.size();
        }
    }
}

// Add food buttons and functions
void MainWindow::on_pushButton_clicked()
{
    if(cityListData.size() > 0)
    {
    QString cityName = ui->comboBox_SelectCityAddFood->currentText();
    QString foodName = ui->lineEdit_FoodName->text();
    double  foodCost = ui->doubleSpinBox_FoodCost->value();

    int count = 0;
    while(count < cityListData.size() - 1 && cityListData[count].getCityName() != cityName)
    {
        count++;
    }
    cityListData[count].addNewFoodItem(foodName, foodCost);

    }

    ui->lineEdit_FoodName->clear();
    ui->doubleSpinBox_FoodCost->clear();
}

//Function for starting a trip beginning at Berlin
void MainWindow::on_OptimalTravel_clicked()
{
    ui->cityListOptimalTravel->clear();
    if(cityListData.empty()) return; //must load file first

    City start = cityListData[1]; //start = berlin
    QVector<City> sorted; //empty vector
    sorted.push_back(start);
    QVector<City> cities = cityListData;
    sorted = recursivePathing(start,cities,sorted);
    double totalDistance = 0;

    //ouput sorted list and total distance traveled
    for(int i = 0; i < sorted.size(); i++){
        ui->cityListOptimalTravel->append(sorted[i].getCityName());
        if(i+1 < sorted.size()){
//            totalDistance += (sorted[i].getCoordinates().distanceTo(sorted[i+1].getCoordinates()));
            totalDistance += (sorted[i].getDistance(sorted[i+1].getCityName()));

        }
    }
     QString Distance = QString::number(totalDistance /*/1000*/ );
     ui->cityListOptimalTravel->append("Total Distance Traveled:" + Distance + "km");
     foodPlanner finalizedTrip;
     finalizedTrip.addTravelPlanData(sorted);
     finalizedTrip.setupUi();
     finalizedTrip.exec();
}

//recursive function to find optimal travel plan based off distances
//start is the city we are traveling from
//cities is the list of cities we can travel to
//sorted is the final product which has the most optimal travel plan
QVector<City> MainWindow::recursivePathing(City start,QVector<City> &cities,QVector<City> &sorted ){
    //deletes starting city from the list of cities
    QVector<City>::iterator it = cities.begin();
    for(int i = 0; i < cities.size(); i++)
    {
        if(start.getCityName() == cities[i].getCityName()){
            cities.erase(it);
        }
        it++;
    }

    //find the closest city to the start city
    City* closest = &cities[0];
    for(int i = 0; i < cities.size();i++)
    {

        if(cities[i].getDistance(start.getCityName()) < closest->getDistance(start.getCityName()))
        {
            closest = &cities[i];
        }
    }
    //add to sorted
    sorted.push_back(*closest);

    //if more than 1 city remains then recurse
    if(cities.size() > 1){
      recursivePathing(*closest,cities,sorted);
    }
    return sorted;
}

void MainWindow::on_customPlan_clicked()
{
    if(ui->comboBoxCities->currentIndex() != 0){
        customPlan customTravel;
        QVector<City> customCityPlan(cityListData);
        City startingCity;

        //This line of code gets the information of the city selected in the drop down menu.
        startingCity = customCityPlan[(ui->comboBoxCities->currentIndex() - 1)];

        //Deletes the starting city from the custom city vector so it wont be selected again
        //inside the custom plan generator.
        customCityPlan.removeAt(ui->comboBoxCities->currentIndex() - 1);

        customTravel.addCity(startingCity);
        customTravel.addCustomCityData(customCityPlan);
        customTravel.setupMenu();

        customTravel.exec();
    }
}

//Function for starting a trip beginning at Paris
void MainWindow::on_OptimalTravel_2_clicked()
{  
    customPlan customTravel;
    QVector<City> customCityPlan(cityListData);
    City startingCity;

    startingCity = customCityPlan[8];   // 8 - index of Paris

    customCityPlan.removeAt(8); // 8 - index of Paris

    customTravel.addCity(startingCity);
    customTravel.addCustomCityData(customCityPlan);
    customTravel.setupMenu();

    customTravel.exec();
}

//This function is called within readData function. It takes the data from distances.txt and updates the distance array in each of the city
//objects for the first 11 cities
void MainWindow::readDistances(QVector<City> &cityList)
{
    //if extended cities are loaded do this
    if(cityList.size() > 11){
     QFile file(":/Files/distancesExt.txt"); //Local file where the data is stored

     //Error Handling if file does not exist
     if(!file.open(QIODevice::ReadOnly))
     {
         QMessageBox::information(0, "info", file.errorString());
     }

     QTextStream in(&file); //Input stream opened

     int i = 0; //Int used to keep track of when to place a zero distance

    //Loop that iterates through the file
     while(!in.atEnd())
     {
         for(int j = 0; j < 13; j++)
         {
            if(j == i)
            {
                cityList[i].getAllDistances()[j] = 0;
            }
            else
            {
                cityList[i].getAllDistances()[j] = in.readLine().toDouble();
            }
         }
         i += 1;
      }
     file.close();
    }

    //if the vector contains only 11 cities
    else{
    QFile file(":/Files/distances.txt"); //Local file where the data is stored

    //Error Handling if file does not exist
    if(!file.open(QIODevice::ReadOnly))
    {
        QMessageBox::information(0, "info", file.errorString());
    }

    QTextStream in(&file); //Input stream opened

    int i = 0; //Int used to keep track of when to place a zero distance


    //Loop that iterates through the file
    while(!in.atEnd())
    {
        for(int j = 0; j < 13; j++)
        {
           if(j == i)
           {
               cityList[i].getAllDistances()[j] = 0;
           }
           else
           {
               cityList[i].getAllDistances()[j] = in.readLine().toDouble();
           }
        }
        i += 1;
     }
    file.close(); //File is then closed
    }

}


void MainWindow::on_pushButton_loadFoods_clicked()
{
    ui->comboBox_EditFood->clear();
    ui->doubleSpinBox_EditFoodPrice->clear();

    if(cityListData.size() > 0)
    {
        QString cityName = ui->comboBox_SelectCityAddFood->currentText();

        int index = 0;
        while(index < cityListData.size() - 1 && cityListData[index].getCityName() != cityName)
        {
            index++;
        }

        QVector<QPair<QString, double>> foods = cityListData[index].getAllFood();

        for(int i = 0; i < foods.size(); ++i){
            ui->comboBox_EditFood->addItem(foods[i].first);
        }
    }

}

void MainWindow::on_pushButton_deleteFood_clicked()
{
    QString cityName = ui->comboBox_SelectCityAddFood->currentText();

    int index = 0;
    while(index < cityListData.size() - 1 && cityListData[index].getCityName() != cityName)
    {
        index++;
    }

    cityListData[index].removeFoodItem(ui->comboBox_EditFood->currentText());

    ui->comboBox_EditFood->clear();
    ui->doubleSpinBox_EditFoodPrice->clear();

}

void MainWindow::on_pushButton_changePrice_clicked()
{
    QString cityName = ui->comboBox_SelectCityAddFood->currentText();

    int index = 0;
    while(index < cityListData.size() - 1 && cityListData[index].getCityName() != cityName)
    {
        index++;
    }

    cityListData[index].changeFoodCost(ui->comboBox_EditFood->currentText(), ui->doubleSpinBox_EditFoodPrice->value());

    ui->comboBox_EditFood->clear();
    ui->doubleSpinBox_EditFoodPrice->clear();
}
