#include "filesys.h"
#include "x86_desc.h"


int8_t index_of_directory=0;




bootblock_t * bootblock_ptr;        // the address of system


// get the address of the file image
// pass it into the bootblock_ptr
// input bootblock_ptr
// output nothing

void initial_filesys(uint32_t file_sys_addr){
        bootblock_ptr=(bootblock_t *)file_sys_addr;
        //return 1;
}


/* function header. this function is used for set the *dentry by finding the 
    same filename in the dir entries.
    input filename fname
          dentry, return address
          if the fname is longer than 32byte set to prev 32 bytes
    return 0 if success
    return -1 if failed  */
int32_t read_dentry_by_name (const uint8_t* fname,dentry_t* dentry){
    int i = 0; //used for iteration, the 'ith' dir entries.
    int length;
    uint8_t fname1[33];
    if (fname[0]=='\0'){return -1;}

    if(strlen((int8_t *)fname ) > 32){return -1;} 
    strncpy((int8_t *)fname1,(const int8_t *)fname,32);
    fname1[32]='\0';
    for(i = 0; i < 63; i++){
        length = strlen(bootblock_ptr->entries[i].name);
        if(length > 32){
            length = 32;
        }
        if( length == strlen((int8_t *)fname1 ) ){
            //if the length of the given 'filename' equals the finding 'filename'
            //printf("%d\n",strlen((int8_t *)fname ));
            
            if(0 == strncmp(bootblock_ptr->entries[i].name,
                            (int8_t *)fname1,
                            length )){
                                //if two filenames are exactly equal to each other
                                strcpy(dentry->name, bootblock_ptr->entries[i].name);
                                dentry->type = bootblock_ptr->entries[i].type;
                                dentry->inode = bootblock_ptr->entries[i].inode;
                                return 0; //success
                            }
        }
    }
    return -1; //not found any filename in the dir entries.
}






// i'm useless//
/*do nothing 
*/
int32_t file_open(const uint8_t* filename){
    return 0;
}

/*
    this function is used for read date based on inode
    input inode
    input offset
    input buf (pointer)
    input len (length )
    return -1 if fail
    return 0 if success

*/

int32_t file_read (int32_t fd, void* buf, int32_t  nbytes){
    uint32_t inode;
    pcb_t *pcb = get_pcbptr();
    int32_t result;
    uint32_t offset=pcb->file_descriptor_table[fd].file_position;
    inode=pcb->file_descriptor_table[fd].inode;
    result=file_read_1(inode,&(offset),(uint8_t*)buf,nbytes);
    pcb->file_descriptor_table[fd].file_position=offset;
    return result;   

}

/*
    close the file
    input fd
    return -1 if fail
    return 0 if success

*/
int32_t file_close (int32_t fd){
    fd=fd;
    return file_close_1();
}
/*
    close the file
    input fd
    return -1 if fail
    return 0 if success

*/
int32_t file_write (int32_t fd, const void* buf, int32_t nbytes){
    return -1;
}
/*
    open the dir
    input fd
    return -1 if fail
    return 0 if success

*/
int32_t dir_open(const uint8_t* filename){
    return dir_open_1();
}
/*
    open the file
    input fd
    return -1 if fail
    return 0 if success

*/
int32_t dir_close (int32_t fd){
    return dir_close_1();
}
/*
    do nothing
    input fd
    return -1 if fail
    return 0 if success

*/
int32_t dir_write (int32_t fd, const void* buf, int32_t nbytes){
    return dir_write_1();
}



// should return the name of the file by order in the directory 
// put name into buf
// return how long the name is in bytes or 0 if out of range
int32_t dir_read (int32_t fd, void* buf, int32_t  nbytes){
    //dir_read_1((uint8_t* )buf,index_of_directory++,sizeof(dentry_t));
    dentry_t temp;
    int8_t length;
    int32_t result;
    int i;
    result=read_dentry_by_index(index_of_directory++,&temp);
    // if the index is out of range
    if (result==-1){
        index_of_directory=0;
        return 0;
    }else
    {
        for (i=0;i<33;i++){
            ((int8_t*)buf)[i]='\0';
        }
        length=strlen((int8_t*)temp.name);
        //printf("%d\n", length);
        if (length<32){
            strncpy((int8_t*)buf,temp.name,length+1);
            ((int8_t*)buf)[length+1]='\0';

        }else
        {   
            strncpy((int8_t*)buf,temp.name,32);
            ((int8_t*)buf)[32]='\0';
            //printf("%s\n",((int8_t*)buf));
            length = 32;
            
        }
        return length;
        
        

    }
    




}


/* function header, this function is used to open a file.
    if we cannot find this filename or this is not a regular file retur -1
    if we succcess, set the inode value in (*inode), and return 0; 
    input filename
    input inode ptr
    
    */
int32_t file_open_1(uint32_t* inode, char* filename){
    dentry_t dentry; //temp used as a dentry
    if(-1 == read_dentry_by_name((uint8_t *)filename, &dentry)){
        //if we could not find this filename in dir entries.
        return -1;
    }
    if(dentry.type != 2){ //'2' stands for regular file
        return -1; //it is not a regular file
    }
    *inode = dentry.inode;
    return 0; //success
}

/* function header, close the file, maybe nothing to do */
int32_t file_close_1(){
    return 0;
}



/* function header 
    read the file based on the inode 
    input inode
    input offset
    input buf (pointer)
    input len (length )
    */
int32_t file_read_1(uint32_t inode, uint32_t* offset, uint8_t* buf,uint32_t len){
    int32_t result = read_data(inode, *offset, buf, len);
    if(result > 0){
        //if the read_data process is successful, update the offset by the result.
        *offset += result;
    }
    return result;
}

/* function header, write to the file, do nothing, return -1 */
int32_t file_write_1(){
    return -1;
}

/*
    open the dir 
    maybe do nothing

*/
int32_t dir_open_1(){

    return 0;
}

/*nothing to do*/
int32_t dir_close_1(){
    return 0;
}
/* function header, this is the function to read the file, length is the
    number of characters we read to the buf, this function will get the name
    of the dir and put it in the buf, the index should be 0 */
int32_t dir_read_1(uint8_t* buf, uint32_t index, uint32_t length){
    dentry_t temp;
    read_dentry_by_index(index,&temp);
    memcpy(buf,temp.name,sizeof(dentry_t));
    return sizeof(dentry_t);
}

//do nothing
int32_t dir_write_1(){
    return -1;
}

/*
 this function is used for set the *dentry by index.
    input index (index of the file)
          dentry, return address
    return 0 if success
    return -1 if failed or the fname is longer than 32byte

*/

int32_t read_dentry_by_index (uint32_t index, dentry_t* dentry){
        if (index>=number_of_entries()){
            return -1;
        }
        *dentry=bootblock_ptr->entries[index];
        return 0;
}   

/*
    this function is used for read date based on inode
    input inode
    input offset
    input buf (pointer)
    input len (length )
    return -1 if fail
    return 0 if success

*/

int32_t read_data(uint32_t inode,uint32_t  offset,uint8_t* buf,uint32_t length){
        inode_t* inode_ptr=(inode_t *)bootblock_ptr + 1+inode ;                     // pointer to the corresponding block
       // int32_t how_many_blocks_the_inode_have=(inode_ptr->length)/BLOCK_SIZE +1; // the size of the file
        int32_t start_block=offset/BLOCK_SIZE;                                      // the first block that the function need read
        int32_t start_address=offset % BLOCK_SIZE;                                   // correspoinding address in the start block
        uint32_t left=BLOCK_SIZE-start_address;                                      // how many left in the block after minus the offset
        uint8_t i=0;
        uint32_t original_len;



        if (inode>(bootblock_ptr->N)){return -1;}                                         // if inode is bigger than the #inode . return fail 
        if (offset>inode_ptr->length){return -1;}                                           //no enough space
        if ((offset+length)>inode_ptr->length){length=inode_ptr->length-offset;}                                 // there is no enought content for the function . fail
        if (!buf) {return -1;}
        original_len=length;
        // first conditon : start_block cotain all the function need
        if (length<=left){                                                                  
                                  
            inode_t* data_ptr=(inode_t *)bootblock_ptr+bootblock_ptr->N+inode_ptr->data_block[start_block]+1; //get the pointer of data block
            uint8_t* src_address=(uint8_t *)data_ptr+start_address;
            memcpy(buf,src_address,length);
        }else{
        //second conditon: start_block do not cotain all the function need
        // we need a loop
        inode_t* data_ptr=(inode_t *)bootblock_ptr+bootblock_ptr->N+inode_ptr->data_block[start_block]+1; 
        uint8_t* src_address=(uint8_t *)data_ptr+start_address;                                             //for the first block do the same thing
        
        memcpy(buf,src_address,left);
       // buf=buf+left;                                                                           // after first copy, buf move forward (left) steps
        length-=left;                                                                           // length decrease
        for (;length>BLOCK_SIZE;length=length-BLOCK_SIZE){                                          
            data_ptr=(inode_t *)bootblock_ptr+bootblock_ptr->N+inode_ptr->data_block[++start_block]+1;        
            src_address=(uint8_t *)data_ptr;                                                          // get the correspoinding blocks 
            memcpy(buf+left+BLOCK_SIZE*i,src_address,BLOCK_SIZE);    
            i++;                                                                // increse the buffer
        }   
        // last block   
        data_ptr=(inode_t *)bootblock_ptr+bootblock_ptr->N+inode_ptr->data_block[++start_block]+1;        
        src_address=(uint8_t *)data_ptr; 
        memcpy(buf+left+BLOCK_SIZE*i,src_address,length);                                                                 // attention the last parameter 

        }
        return original_len;

        


        
}

/*
member function
get the number of entries
*/
int32_t number_of_entries(){
    return bootblock_ptr->number_entries;
}


/*
member function
get the size of the file based on inode
input inode
*/

int32_t size_of_file(uint32_t inode){
    inode_t* inode_ptr=(inode_t *)bootblock_ptr + 1+inode;
    return inode_ptr->length;
}
