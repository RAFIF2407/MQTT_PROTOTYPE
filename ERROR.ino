void ERROR(){
if(WiFi.status() != WL_CONNECTED){
  if(internet==0){
    if(!client.connected()){
      if(suhu==0){
        error=0;
      }
      else if(suhu>0){
        error=1;
      }
    }
    else if(client.connected()){
      if(suhu==0){
        error=2;
      }
      else if(suhu>0){
        error=3;
      }
    }
  }
  else if(internet==1){
    if(!client.connected()){
      if(suhu==0){
        error=4;
      }
      else if(suhu>0){
        error=5;
      }
    }
    else if(client.connected()){
      if(suhu==0){
        error=6;
      }
      else if(suhu>0){
        error=7;
      }
    }
  }
}

else {
  if(internet==0){
    if(!client.connected()){
      if(suhu==0){
        error=8;
      }
      else if(suhu>0){
        error=9;
      }
    }
    else if(client.connected()){
      if(suhu==0){
        error=10;
      }
      else if(suhu>0){
        error=11;
      }
    }
  }
  else if(internet==1){
    if(!client.connected()){
      if(suhu==0){
        error=12;
      }
      else if(suhu>0){
        error=13;
      }
    }
    else if(client.connected()){
      if(suhu==0){
        error=14;
      }
      else if(suhu>0){
        error=15;
      }
    }
  }
}

}//END OF VOID
