docker run -it --name mosquitto -p 1883:1883 
    -v $(pwd)/mosquitto:/mosquitto/ eclipse-mosquitto
    
    
    
    
allow_anonymous false
listener 1883
persistence true
persistence_location /mosquitto/data/
password_file /mosquitto/pass


mosquitto_passwd -c passwordfile user

