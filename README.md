Ce projet contient un jeu de BlindTest, proposant deux playlists (Années 80 & Années 2010), l'objectif est d'avoir 20 points (trouver 20 bonnes réponses) le plus rapidement possible (calculé en secondes).


Pour lancer le BlindTest, veuillez executer cette ligne de commande : `gcc -o main main.c get_playlist.c get_song.c quiz_page.c globals.c create_database.c connect_db.c menu.c leaderboard.c open_config.c winning_page.c -ljansson -lcurl -lsqlite3 -Wno-deprecated-declarations $(pkg-config --cflags --libs gtk+-3.0 gstreamer-1.0)`

Les installations nécéssaires : 

- apt-get install gcc
- apt-get install libgtk-3-dev
- apt-get install libcurl4-gnutls-dev
- apt-get install libssl-dev
- apt-get install libsqlite3-dev
- apt-get install libxml2
- apt-get install libxml2-dev
- apt-get install libjansson-dev
- apt-get install libcurl4-openssl-dev
- apt-get install gstreamer1.0
- apt-get install libgstreamer1.0-dev
- apt-get install libgstreamer-app1.0-dev
- apt-get install gstreamer1.0-plugins-base
- apt-get install gstreamer1.0
- apt-get install libgstreamer1.0-dev libgstreamer-plugins-base1.0-dev
- apt-get install gstreamer1.0-plugins-base gstreamer1.0-plugins-good
