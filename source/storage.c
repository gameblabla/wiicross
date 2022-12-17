#include "../include/wiicross.h"
#include "../include/storage.h"

extern s_level leveldata[NUMLEVELS];
extern s_option options;

s_level leveldata_tmp[NUMLEVELS]; //HACK FOR FIX BUGGED LEVELS

char error[1000];

struct _reent *r_all;

void initStorage(){
	
	if(!fatInitDefault()){
		sprintf(error, "failed fat init default: %s", strerror(errno));
		//breakpoint(error, errno);
		//breakpoint("failed fat init default ", errno);
		return;
	}
	
	DIR* dir = opendir(DIR_ROOT "res/"); 
	if (dir == NULL) {
		mkdir(DIR_ROOT "res", S_ISVTX);
			//breakpoint("can't create res directory", errno);
	}
	else
		closedir(dir);
	
	if(!readSD(FILE_LEVELS))
		writeSD(FILE_LEVELS);
	
	if(!readSD(FILE_OPTIONS))
		writeSD(FILE_OPTIONS);
	
	return;
}

int writeSD(int file){
	
	FILE *fp = NULL;
	
	if(file == FILE_LEVELS)
		fp = fopen(DIR_ROOT "res/levels.dat","wb");
	else if (file == FILE_OPTIONS)
		fp = fopen(DIR_ROOT "res/options.dat","wb");
	else // FILE_RESUME
		breakpoint("che cazzo mi fai scrivere",71);	

	if(!fp){
		sprintf(error, "failed to write: %s", strerror(errno));
		//breakpoint(error, errno);
		//breakpoint("Can't write, errno: ", errno);
		return -1;
	}
	
	if(file == FILE_LEVELS)
		fwrite(leveldata, sizeof(leveldata[0]), NUMLEVELS, fp);
	else if (file == FILE_OPTIONS){
		fwrite(&options, sizeof(s_option), 1, fp);
	}
	else
		breakpoint("che cazzo mi fai scrivere p2",72);
		
	fclose(fp);
	
	return 0;
}

int readSD(int file){

	//return  -1;

	FILE *fp = NULL;
	
	if(file == FILE_LEVELS)
		fp = fopen(DIR_ROOT "res/levels.dat","r");
	else if (file == FILE_OPTIONS)
		fp = fopen(DIR_ROOT "res/options.dat","r");
	else // FILE_RESUME
		breakpoint("che cazzo mi fai caricare",69);

	if(!fp){
		//breakpoint("Can't read, errno: ", errno);
		sprintf(error, "failed to read: %s", strerror(errno));
		//breakpoint(error, errno);
		return -1;
	}

	if(file == FILE_LEVELS){
		//fread(leveldata, sizeof(leveldata[0]), NUMLEVELS, fp);
		fread(leveldata_tmp, sizeof(leveldata[0]), NUMLEVELS, fp);
		int i;
		for(i=0; i<NUMLEVELS; i++){
			leveldata[i].played = leveldata_tmp[i].played;
		}
	}
	else if(file == FILE_OPTIONS){
		fread(&options, sizeof(s_option), 1, fp);
	}
	else
		breakpoint("che cazzo mi fai caricare p2",70);
	
	fclose(fp);
	
	return 0;
}

void saveCurrentLevel(){

}

int readThemesDir(){
	
	bool bgFound = false;
	bool markedFound = false;
	bool filledFound = false;
	bool lCompleteFound = false;
	bool titleFound = false;
	
	struct stat st;
	struct stat st2;
	char dirname[MAXPATHLEN];
	
	char temp[1000];
	int spriteError = 0;
	struct dirent *entry;
	struct dirent *entry2;
	DIR* dir;
	DIR* themeDir;

	dir = opendir (DIR_ROOT "res/themes");
	
	int themeCounter = 0;
	sprintf(theme.themesArray[themeCounter], "default");
	themeCounter++;
	sprintf(theme.themesArray[themeCounter], "woods");
	themeCounter++;
	sprintf(theme.themesArray[themeCounter], "space");
	themeCounter++;
	sprintf(theme.themesArray[themeCounter], "candy");
	themeCounter++;
	sprintf(theme.themesArray[themeCounter], "spooky");
	themeCounter++;
	sprintf(theme.themesArray[themeCounter], "blue");
	themeCounter++;
	
	if(dir == NULL) {
		//breakpoint("Unable to generic theme the directory.", errno);
		closedir(dir);
		return themeCounter;
		//return -1;
	}
	
		
	while ((entry = readdir(dir)) != NULL){
		stat(entry->d_name,&st);
		
		snprintf(temp, sizeof(temp), "%s", entry->d_name);
		
		// is it a directory and it is not . nor ..?
		if((strlen(temp)>2) && (st.st_mode & S_IFDIR)){
					
			// we found first (supposed) theme dir
			sprintf(dirname, DIR_ROOT "res/themes/%s", temp);
			themeDir = opendir (dirname);
			
			if(themeDir == NULL){
				//breakpoint("Unable to open the THEME directory.\n", errno);
				return -3;
			}
			
			bgFound = false;
			markedFound = false;
			filledFound = false;
			lCompleteFound = false;
			titleFound = false;
			
			// we are IN the first (supposed) theme dir
			while ((entry2 = readdir(themeDir)) != NULL){				
				stat(entry->d_name,&st2);
							
				if((strlen(entry2->d_name)>2) && (st2.st_mode == 33206)){

					if(strcasecmp(entry2->d_name, "bg.png") == 0){
						bgFound = true;
					}
						
					if(strcasecmp(entry2->d_name, "marked.png") == 0){
						markedFound = true;
					}
						
					if(strcasecmp(entry2->d_name, "filled.png") == 0){
						filledFound = true;
					}
					
					if(strcasecmp(entry2->d_name, "levelcomplete.png") == 0){
						lCompleteFound = true;
					}
					
					if(strcasecmp(entry2->d_name, "title.png") == 0){
						titleFound = true;
					}
				}
			}
			
			//0 false, 1 true		
			if(bgFound & markedFound & filledFound & lCompleteFound & titleFound){
				if(themeCounter < 255){		
					sprintf(theme.themesArray[themeCounter], temp);
					sprintf(temp, DIR_ROOT "res/themes/%s/title.png", theme.themesArray[themeCounter]);
					spriteError = createSprite(&theme.titleArray[themeCounter], NULL, temp, 508, 263, 1, FMT_PNG, true);
					
					if( spriteError != 1)
						breakpoint("cannot load resource filled ", spriteError);
					else
						themeCounter++;
				}
			}
			
			closedir(themeDir);
		}
	}
	
	closedir(dir);
	return themeCounter;
}

