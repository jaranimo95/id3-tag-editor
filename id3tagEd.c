//Christian Jarani
//Prof. Misurda
//CS 449: Project 1
//2/9/2016

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ID3 Tag Structure Declaration */
struct id3tag
{
    char tag[3];
    char title[30];
    char artist[30];
    char album[30];
    char year[4];
    char comment[28];
    char separator;
    char track;
    char genre;
};

/* Prototype Declarations */
void read_tag();
void edit_tag();
void write_tag();

int main(int argc, char *argv[])
{    
    struct id3tag t;    
    char full_tag[128];
    
   //Read Mode: read id3 tag from file in its entirety into full_tag array, then read from full_tag to id3tag structure
    FILE *fr = fopen(argv[1],"rb");
     fseek(fr, -128, SEEK_END);
      fread(full_tag, 1, 128, fr);
       read_tag(&t, full_tag);   
        fclose(fr);

	char tag_fix[4];		//TAG has no sentinel value in the id3 tag, so we must add one so it can be printed
	strncpy(tag_fix, t.tag, 3);
	tag_fix[3] = '\0';
    
   //No Tag Mode
    if((strcmp(tag_fix,"TAG\0") != 0) && argc >  2)
    {
        edit_tag(1, argc, argv, &t);
        write_tag(&t, full_tag);

       //Reopen for appending
    	FILE *fa = fopen(argv[1],"ab");
    	fseek(fa, 0, SEEK_END);
    	fwrite(full_tag, 1, 128, fa);
    	fclose(fa);

	    return 0;
    }

   //Print Mode
    if((strcmp(tag_fix,"TAG\0") == 0) && argc == 2) //If no other values (besides the filename) were passed into the program
    {
    	char year_fix[5];		//Year has no null sentinel in the id3 tag, so we must add one so it can be printed
    	strncpy(year_fix, t.year, 4);
    	year_fix[4] = '\0';
	  
       //Print out contents of id3tag structure
        printf("Title:   %s\n", t.title);
        printf("Artist:  %s\n", t.artist);
        printf("Album:   %s\n", t.album);
        printf("Year:    %s\n", year_fix);
    	printf("Comment: %s\n", t.comment);
    	printf("Track:   %d\n", atoi(&t.track));
    }

   //Edit/Write Mode: If the tag and 2+ arguments exist, (FIELD,DATA) pairs must be appended to tag
    if((strcmp(tag_fix,"TAG\0") == 0) && argc > 2)
    {
        edit_tag(0, argc, argv, &t);
        write_tag(&t, full_tag);

       //Reopen for appending
        FILE *fw = fopen(argv[1], "ab");
        fseek(fw, -128, SEEK_END);
        fwrite(full_tag, 1, 128, fw); 
        fclose(fw);
    } 
      return 0;
}


  /*-------------------------------*/
 /* Built-In Function Definitions */
/*-------------------------------*/

/* Reads data from full_tag array (pointed to by *p) and assigns to each id3tag structure field
 * @param: the id3tag structure, char pointer p (holds address of full_tag array)
 */

void read_tag(struct id3tag *t, char *p)
{
    int i;
    for(i = 0; i < 128; i++)
    {
        if(i < 3) 			        //TAG Identifier (3 bytes, offset of 0)
            t->tag[i] = p[i];
        if((i > 2) && (i < 33))                 //Title (30 bytes, offset of 3)
            t->title[i-3] = p[i];
        if((i > 32) && (i < 63))	   	//Artist (30 bytes, offset of 33
            t->artist[i-33] = p[i];
        if((i > 62) && (i < 93))	        //Album (30 bytes, offset of 63)
            t->album[i-63] = p[i];
        if((i > 92) && (i < 97))	        //Year (4 bytes, offset of 93)
            t->year[i-93] = p[i];
        if((i > 96) && (i < 125))	        //Comment (28 bytes, offset of 97)
            t->comment[i-97] = p[i];
        if(i == 125)		      		//Zero Separator (1 byte, offset of 125)
            t->separator = p[i];
        if(i == 126)		       		//Track Number (1 byte, offset of 126
            t->track = p[i];
        if(i == 127)		        	//Genre Identifier (1 byte, offset of 
            t->genre = p[i];
    }  
}

/* Allows for the editing of a pre-existing tag
 * @param: no. of arguments, arguments passed into program, id3 tag structure
 */

void edit_tag(int mode, int argc, char *argv[], struct id3tag *t)
{
   //Used to read (FIELD,VALUE) pairs
    int count = 2;    
   
   //Used by No Tag Mode to determine which fields to set to zero
    int title_edit = 0;
    int artist_edit = 0;
    int album_edit = 0;
    int year_edit = 0;
    int comment_edit = 0;
    int track_edit = 0;

    while((count < argc) && (argv[count][0] == '-')) //Edit contents of specified field
    {
       //Check first character of FIELD from input
	switch(argv[count][1])
	{	    
	    case 't':
	       //Check second character of FIELD from input
		if(argv[count][2] == 'i')
		{   
		    //edit t.title
		     strncpy(t->title, argv[count+1], 30);
		     if(mode == 1)
			title_edit++;
		      break;
		}   
		if(argv[count][2] == 'r')
		{   
		    //edit t.track
		     strncpy(&t->track, argv[count+1], 1);
		     if(mode == 1)
			track_edit++;
		      break;
		}   
		  break;
    
	    case 'a':
		if(argv[count][2] == 'r')
		{   
		    //edit t.artist
		     strncpy(t->artist, argv[count+1], 30);
		     if(mode == 1)
			artist_edit++;
		      break;
		}   
		if(argv[count][2] == 'l')
		{   
		    //edit t.album
		     strncpy(t->album, argv[count+1], 30);
		     if(mode == 1)
			album_edit++;
		      break;
		}   
		  break;   

	    case 'y':
		//edit t.year
		 strncpy(t->year, argv[count+1], 4);
		 if(mode == 1)
		    year_edit++;
		  break;
		    
	    case 'c':
		//edit t.comment
		 strncpy(t->comment, argv[count+1], 28);
		 if(mode == 1)
		    comment_edit++;
		  break;
		    
	    default: break;	    
	}	        
	 count += 2;
    }

    if(mode == 1) //Whichever fields haven't been edited, set all addresses to 0
    {
	strncpy(t->tag, "TAG", 3);

        if(title_edit < 1)
	    strncpy(t->title, "0", 30);
        if(artist_edit < 1)
	    strncpy(t->artist, "0", 30);
        if(album_edit < 1)
	    strncpy(t->album, "0", 30);
        if(year_edit < 1)
	    strncpy(t->year, "0", 4);
        if(comment_edit < 1)
	    strncpy(t->comment, "0", 28);
        if(track_edit < 1)
	    strncpy(&t->track, "0", 1);
    }
}

/* Combines all id3tag structure data into the full_tag array via char pointer *p
 * @param: the id3tag structure, char pointer p (holds address of full_tag array)
 */

void write_tag(struct id3tag *t, char *p)
{
    int i;
    for(i = 0; i < 128; i++)
    {
        if(i < 3)		   //Refer to read_tag method for array assignment
            p[i] = t->tag[i];
        if((i > 2) && (i < 33))
	    p[i] = t->title[i-3];
        if((i > 32) && (i < 63))
	    p[i] = t->artist[i-33];
        if((i > 62) && (i < 93))
	    p[i] = t->album[i-63];
        if((i > 92) && (i < 97))
	    p[i] = t->year[i-93];
        if((i > 96) && (i < 125))
	    p[i] = t->comment[i-97];
        if(i == 125)
	    p[i] = t->separator;
        if(i == 126)
	    p[i] = t->track;
        if(i == 127)
	    p[i] = t->genre;
    }    
}
