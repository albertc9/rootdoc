
void pile_up(){

    // define test time, unit: ms
    const int all=2000000;
    // {ile-up event ratio
    int pile_up=0;
    // record occur event number
    int Event_all=0;

    // Poisson parameter define
    double event_ratio=60; // 60 Hz
    // change Poisson parameter from s to ms;
    double lamda=event_ratio*0.001;



    //record event occur event number with time
    int event_flag[all]={0};
    //record work state with time
    int work_flag[all]={0};

    for(int i=0;i<all;++i){
    int temp=gRandom->Poisson(lamda);
    if(temp){
        Event_all++;
        event_flag[i]=temp;
    // if detector is not working, change work state in next 3 ms;
        if(!work_flag[i]){
        work_flag[i]=1;
        if (i + 1 < all) work_flag[i + 1]=1;
        if (i + 2 < all) work_flag[i + 2]=1;
        }
        else pile_up++;
    }
    }

    double pile_up_rate = pile_up / static_cast<double>(Event_all);

    // output to check
    /*
    for(int i=0;i<1000;++i){
    cout<<event_flag[i]<<"  "<<work_flag[i]<<endl;
    }
    */

    cout<<"Event All: "<<Event_all<<"; Pile-up Event "<<pile_up<<"; Time All: "<<all<<" ms"<<endl;
    // output pile-up ratio && check if the event ratio equal to define ratio
    cout << "Pile-up Rate: " << pile_up_rate << endl;
    cout<<"pile_up possibility:  "<<pile_up/1./all<<"; Event ratio: "<<Event_all/1./all*1000<<endl;

    }