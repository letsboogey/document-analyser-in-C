#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

/*
 C PROGRAMMING COURSEWORK [Word frequency counter]

 Description :  
 This program reads in a document given as the first argument in the command line
 and stores the text in a string. The text is then stripped of all special
 characters and numbers, then it is split into individual words which are then 
 stored in a linked list. Each node of the list contains a word and its frequency.
 As the words are inserted in the linked list, the frequency is incremented if the given 
 word is already present in the list. The program then saves the node data(word,freq)
 in a comma-separated-value(csv) file given as the second argument in the command line. 
 They are saved in decreasing order of frequency

*/


//define the node data type and call it 'Node'
typedef struct NodeStructure{
    char *word;
    int freq;
    struct NodeStructure *next;
} Node;

//create an empty linked list
Node *freq_List = NULL;

/*inserts a word into the frequency linked list*/
void insert(char *theWord, Node **head){

    Node *newNode = *head;

    while( newNode != NULL ){
        if( strcmp(newNode->word, theWord ) == 0 ){
                newNode->freq++;
                return;
            }
            newNode = newNode->next;
    }

    //allocate memory to new node
    newNode = (Node*)malloc(sizeof(Node));

    //check if memory allocation is successful
    if( newNode == NULL ){
        fprintf(stderr, "Problem allocating memory for new node\n");
        exit(EXIT_FAILURE);
    }

    //add data to new node
    newNode->word = theWord;
    newNode->freq = 1;
    newNode->next = *head; 

    *head = newNode;
    return;
}


/*return the number of nodes in the frequency list*/
int listSize(){

    Node *current = freq_List;
    int size = 0;
    
    while( current != NULL ){
        size++;
        current = current->next;
    }
    
    printf("\nSize of Linked List : %d nodes\n",size);

    return size;
}



/*reads file contents to a string*/
char *readFile(const char *filename){

    char *fcontent = NULL;
    int fileSize = 0;
    FILE *theFile;

    theFile = fopen(filename, "r");
    
    if( theFile == NULL ){ //print error message if file cannot be opened
        printf(" Error : '%s' could not be opened or does not exist.\n" ,filename);
        exit(EXIT_FAILURE);
    }else{ 
    	//determine the file size in bytes
        fseek(theFile, 0, SEEK_END);
        fileSize = ftell(theFile);
        rewind(theFile);

        //deal with empty file
        if(fileSize == 0){
            printf(" Error : '%s' is an empty file.\n" ,filename);
            exit(EXIT_FAILURE);
        }

        //allocate memory to a string and write the characters to it
        fcontent = (char*) malloc (sizeof(char) * fileSize);
        fread(fcontent, 1, fileSize, theFile);
        fclose(theFile);
    }
	return fcontent;
}


/*
splits text into individual words and inserts them in linked list
through the insert method
*/
void *filter_out_Words(char *document){
    
    //convert document contents to lowercase
    int i;
    for( i = 0 ; document[i] ; i++ ){
        document[i] = tolower(document[i]);
    }
    
    //remove all special characters and numbers
    int j,k;    
    for( j = 0 ; document[j] ; j++ ){
        while ( !(isalpha(document[j]) || isspace(document[j]) || document[j] == '\0') ){
            for( k = j ; document[k] != '\0' ; ++k ){
                document[k] = document[k+1];
            }
        }
    }
    printf("%s",document);

    //split document at any whitespace and insert words into linked list
    char *word;    
    word = strtok( document,"'\n' '\t'" );
    while ( word != NULL ){
        insert(word,&freq_List);
        word = strtok( NULL, "'\n' '\t'" );
    }    
}


/*Returns the node containing the word with the largest frequency in the list*/
Node *largest(){

    Node *largest,*head;
    largest = head = freq_List;
    
    while(head != NULL){
        if( head->freq >= largest->freq ){
            largest = head;            
        }        
        head = head->next;
    }
   
    return (largest);
}


/*removes the node which contains the word given as an argument*/
void delete(char *theWord){
   
    Node *current, *temp;
    current = freq_List;
    
    //if word is contained in first node, delete node and set head to next node
    if( strcmp(current->word, theWord) == 0 ){
        freq_List = current->next;
        free(current);
        return;
    }else{
         //if first node did not contain the word, search through all nodes and update the pointers
        while(current != NULL){
            if( strcmp(current->word, theWord) == 0 ){
                temp->next = current->next;               
                free(current);
                return;
            }else{
                temp = current;//keep track of previous node before testing next
                current = current->next;
            }
        }
    }
}


/*
 saves the words and respective frequencies in decreasing order
 using largest() method to determine which node data to print
*/
void save(char *filename){   
    
    FILE *theFile;
    //create a file to write the data to
    theFile = fopen(filename, "w+");
    
    //print error message and exit program if there is a problem creating the file
    if( theFile == NULL ){ 
        printf(" Error : '%s' could not be opened.\n" ,filename);
        exit(EXIT_FAILURE);
    }else{ 
        //print the column headers for the file
        fprintf(theFile, "%s,%s \n","WORD","FREQUENCY" );
        
        int size = listSize();
        while( size > 0 ){

            Node *max = largest();
            fprintf(theFile, "%s,%d \n",max->word, max->freq );
            delete(max->word);//remove node from list to make way for next largest freq
            size--;
        }
    }
}


//main function
int main( int argc, char *argv[] ){
    
    if ( argc != 3 ){
        printf("Exactly two argument expected. i.e: document filepath and output csv filename\n");
    }else{
        char *filepath = argv[1];
        char *csv_fname = argv[2];

        char *file_contents = readFile(filepath);
       // puts(file_contents);
        filter_out_Words(file_contents);      
        save(csv_fname);
        free(file_contents);          
    }    
    return 0;
}