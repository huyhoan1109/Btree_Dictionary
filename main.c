#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "gtk-3.0/gtk/gtk.h"
#include "btree-al/inc/btree.h"

typedef struct {
    GtkWidget *dialogAdd;
    GtkWidget *addEntry_word;
    GtkWidget *addEntry_mean;
    GtkWidget *dialogDelete;
    GtkWidget *dialogAbout;
    GtkWidget *Pick_File;
    GtkWidget *Ok_msg;
    GtkWidget *Err_msg;
    GtkWidget *Search_show;
    GtkWidget *Search_mean;
    GtkWidget *dialogFix;
    GtkWidget *fixEntry_mean;
}app_widgets;

GtkWidget *searchEntry;
GtkWidget *fixEntry_word;
GtkWidget *deleteEntry;

BTA *eng_dict = NULL;
GtkListStore *list;
GtkEntryCompletion *completion;
GtkEntryCompletion *completionD;
GtkEntryCompletion *completionF;
int kiemtra(char *word){
    for(int i=0;strlen(word)>i;i++){
        if(word[i]==' ')continue;
        if(0<atoi(word+i)&&atoi(word+i)<10||word[i]-'0'==0) return 0;
    }
    return 1;
}
void opendict()
{
    if((eng_dict = btopn("team4_dict", 0, 0)) == NULL)// Ham open
    {
        eng_dict = btcrt("team4_dict", 0, 0);// Ham create
    }
}

void closedict()
{
    if(eng_dict != NULL)
    {
        btcls(eng_dict);//Ham close
    }
}

void make_line(char str[]){
    for(int i =0;strlen(str)>i;i++){
        if(str[i]==' ') str[i] = '_';
    }
}

void clearstr(char *str){
    for(int i =0;strlen(str)>i;i++){
        if(str[i]=='_') str[i] = ' ';
    }
}

static void get_widgets(app_widgets *wdgts,GtkBuilder *builder){
    wdgts->addEntry_word = GTK_WIDGET(gtk_builder_get_object(builder,"addEntry_word"));
    wdgts->addEntry_mean = GTK_WIDGET(gtk_builder_get_object(builder,"addEntry_mean"));
    wdgts->dialogAdd = GTK_WIDGET(gtk_builder_get_object(builder,"dialogAdd"));

    wdgts->dialogAbout = GTK_WIDGET(gtk_builder_get_object(builder,"dialogAbout"));
    wdgts->dialogDelete = GTK_WIDGET(gtk_builder_get_object(builder,"dialogDelete"));
    deleteEntry = GTK_WIDGET(gtk_builder_get_object(builder,"deleteEntry"));

    wdgts->Err_msg = GTK_WIDGET(gtk_builder_get_object(builder,"Err_msg"));
    wdgts->Ok_msg = GTK_WIDGET(gtk_builder_get_object(builder,"Ok_msg"));
    wdgts->Pick_File = GTK_WIDGET(gtk_builder_get_object(builder,"Pick_File"));

    wdgts->Search_show = GTK_WIDGET(gtk_builder_get_object(builder,"Search_show"));
    wdgts->Search_mean = GTK_WIDGET(gtk_builder_get_object(builder,"Search_mean"));
    searchEntry = GTK_WIDGET(gtk_builder_get_object(builder,"searchEntry"));

    wdgts->dialogFix = GTK_WIDGET(gtk_builder_get_object(builder,"dialogFix"));
    fixEntry_word = GTK_WIDGET(gtk_builder_get_object(builder,"fixEntry_word"));
    wdgts->fixEntry_mean = GTK_WIDGET(gtk_builder_get_object(builder,"fixEntry_mean"));

    list = GTK_LIST_STORE(gtk_builder_get_object(builder,"liststore"));
    completion = GTK_ENTRY_COMPLETION(gtk_builder_get_object(builder,"entrycompletion"));
    completionD = GTK_ENTRY_COMPLETION(gtk_builder_get_object(builder,"completionD"));
    completionF = GTK_ENTRY_COMPLETION(gtk_builder_get_object(builder,"completionF"));
}
void thanh_cong(app_widgets *wdgts){
    gtk_message_dialog_set_markup(GTK_MESSAGE_DIALOG(wdgts->Ok_msg), "Thành công!");
    gtk_dialog_run(GTK_DIALOG(wdgts->Ok_msg));
    gtk_widget_hide(wdgts->Ok_msg);
}

void that_bai(app_widgets *wdgts){
    gtk_message_dialog_set_markup(GTK_MESSAGE_DIALOG(wdgts->Err_msg), "Thất bại mời quý khách hãy kiểm tra lại từ!");
    gtk_dialog_run(GTK_DIALOG(wdgts->Err_msg));
    gtk_widget_hide(wdgts->Err_msg);
}
void match_selected(GtkEntryCompletion *widget,GtkTreeModel *model,GtkTreeIter *iter, app_widgets *wdgt)
{
    GValue value = {0, };//Tao thanh 1 cot
    const char *word;
    gtk_tree_model_get_value(model,iter, 0,&value);
    word = g_value_get_string(&value);
    gtk_entry_set_text(GTK_ENTRY(searchEntry),word);
    g_value_unset(&value);
}

void list_for_auto_complete(){
    char word[100],mean[100];
    int i;
    GtkTreeIter iter;
    btpos(eng_dict,1);

    gtk_entry_completion_set_text_column(completion,0);
    gtk_entry_set_completion(GTK_ENTRY(searchEntry),completion);
    g_signal_connect(G_OBJECT(completion),"match-selected",G_CALLBACK(match_selected),NULL);


    list = gtk_list_store_new(1,G_TYPE_STRING);
    while(btseln(eng_dict,word,mean,100,&i)==0)
    {
        gtk_list_store_append(list,&iter);
        gtk_list_store_set(list,&iter,0,word,-1);
    }
    gtk_entry_completion_set_model(completion,GTK_TREE_MODEL(list));
}

void take_Event_press(GtkWidget *widget, GdkEventKey *key,app_widgets *wdgts){
    opendict();
    list_for_auto_complete();
    if(key){
        gchar word[100];
        strcpy(word,(gchar*)gtk_entry_get_text(GTK_ENTRY(widget)));
        gchar mean[100];
        gint i;
        if(key->keyval == GDK_KEY_Return)// Tra ve gia tri tim kiem
        {
            if(strlen(word)<=0||kiemtra(word)==0){
                that_bai(wdgts);
            }
            else{
                if(bfndky(eng_dict,word,&i)==0)
                {
                    btsel(eng_dict,word,mean,100,&i);
                    gtk_label_set_text(GTK_LABEL(wdgts->Search_show),word);
                    gtk_label_set_text(GTK_LABEL(wdgts->Search_mean),mean);
                }
                else
                {
                    that_bai(wdgts);
                }
            }
        }
        else if(key->keyval == GDK_KEY_Tab){
            char key[100],mean[100];
            int size = 0;
            btpos(eng_dict,1);
            while(btseln(eng_dict, key, mean,100, &size) == 0)
                if(strncmp(word, key, strlen(word)) == 0)
                {
                    gtk_entry_set_text(GTK_ENTRY(widget), key);
                    break;
                }
        }
        else if(key->keyval == GDK_KEY_BackSpace || key->keyval == GDK_KEY_Delete)
            {
                gtk_label_set_text(GTK_LABEL(wdgts->Search_show), "");
                gtk_label_set_text(GTK_LABEL(wdgts->Search_mean), "");
            }
        }
    closedict();
}

void ADD_WORD(app_widgets *wdgts){
    opendict();
    gchar *word = (gchar*)malloc(sizeof(gchar));
    strcpy(word,(gchar*)gtk_entry_get_text(GTK_ENTRY(wdgts->addEntry_word)));
    gchar *mean = (gchar*)malloc(sizeof(gchar));
    strcpy(mean,(gchar*)gtk_entry_get_text(GTK_ENTRY(wdgts->addEntry_mean)));
    gint res;
    if(strlen(word)<=0||strlen(mean)<=0||kiemtra(word)==0||kiemtra(mean)==0){
        that_bai(wdgts);
    }
    else{
        if(bfndky(eng_dict,word,&res)==0)
        {
            that_bai(wdgts);
        }
        else
        {
            btins(eng_dict,word,mean,100);
            thanh_cong(wdgts);
        }
    }
    gtk_entry_set_text(GTK_ENTRY(wdgts->addEntry_word), "");
    gtk_entry_set_text(GTK_ENTRY(wdgts->addEntry_mean), "");
    closedict();
}

void match_selectedF(GtkEntryCompletion *widget,GtkTreeModel *model,GtkTreeIter *iter, app_widgets *wdgt)
{
    GValue value = {0, };//Tao thanh 1 cot
    const char *word;
    gtk_tree_model_get_value(model,iter, 0,&value);
    word = g_value_get_string(&value);
    gtk_entry_set_text(GTK_ENTRY(fixEntry_word),word);
    g_value_unset(&value);
}

void list_for_auto_completeF(){
    char word[100],mean[100];
    int i;
    GtkTreeIter iter;
    btpos(eng_dict,1);

    gtk_entry_completion_set_text_column(completionF,0);
    gtk_entry_set_completion(GTK_ENTRY(fixEntry_word),completionF);
    g_signal_connect(G_OBJECT(completionF),"match-selected",G_CALLBACK(match_selectedF),NULL);


    list = gtk_list_store_new(1,G_TYPE_STRING);
    while(btseln(eng_dict,word,mean,100,&i)==0)
    {
        gtk_list_store_append(list,&iter);
        gtk_list_store_set(list,&iter,0,word,-1);
    }
    gtk_entry_completion_set_model(completionF,GTK_TREE_MODEL(list));
}

void FIX_WORD_take_word(GtkWidget *widget, GdkEventKey *key,app_widgets *wdgts){
    // Kha giong vs tim kiem tu vung kho qua chua nghi ra
    opendict();
    list_for_auto_completeF();
    if(key){
        gchar word[100],mean[100];
        strcpy(word,(gchar*)gtk_entry_get_text(GTK_ENTRY(widget)));
        gint i;
        if(key->keyval == GDK_KEY_Return)// Tra ve gia tri tim kiem
        {
            if(strlen(word)<=0||kiemtra(word)==0){
                that_bai(wdgts);
            }
            else{
                if(bfndky(eng_dict,word,&i)==0)
                {
                    btsel(eng_dict,word,mean,100,&i);
                    gtk_entry_set_text(GTK_ENTRY(fixEntry_word),word);
                    gtk_entry_set_text(GTK_ENTRY(wdgts->fixEntry_mean),mean);
                }
                else
                {
                    that_bai(wdgts);
                }
            }
        }
        else if(key->keyval == GDK_KEY_Tab){
            char key[100],mean[100];
            int size = 0;
            btpos(eng_dict,1);
            while(btseln(eng_dict, key, mean,100, &size) == 0)
                if(strncmp(word, key, strlen(word)) == 0)
                {
                    gtk_entry_set_text(GTK_ENTRY(widget), key);
                    break;
                }
        }
    }
    closedict();
}

void match_selectedD(GtkEntryCompletion *widget,GtkTreeModel *model,GtkTreeIter *iter, app_widgets *wdgt)
{
    GValue value = {0, };//Tao thanh 1 cot
    const char *word;
    gtk_tree_model_get_value(model,iter, 0,&value);
    word = g_value_get_string(&value);
    gtk_entry_set_text(GTK_ENTRY(deleteEntry),word);
    g_value_unset(&value);
}

void list_for_auto_completeD(){
    char word[100],mean[100];
    int i;
    GtkTreeIter iter;
    btpos(eng_dict,1);

    gtk_entry_completion_set_text_column(completionD,0);
    gtk_entry_set_completion(GTK_ENTRY(deleteEntry),completionD);
    g_signal_connect(G_OBJECT(completionD),"match-selected",G_CALLBACK(match_selectedD),NULL);


    list = gtk_list_store_new(1,G_TYPE_STRING);
    while(btseln(eng_dict,word,mean,100,&i)==0)
    {
        gtk_list_store_append(list,&iter);
        gtk_list_store_set(list,&iter,0,word,-1);
    }
    gtk_entry_completion_set_model(completionD,GTK_TREE_MODEL(list));
}

void DELETE_WORD_take_word(GtkWidget *widget, GdkEventKey *key,app_widgets *wdgts){
    opendict();
    list_for_auto_completeD();
    if(key){
        gchar word[100];
        strcpy(word,(gchar*)gtk_entry_get_text(GTK_ENTRY(widget)));
        gint i;
        if(key->keyval == GDK_KEY_Return)// Tra ve gia tri tim kiem
        {
            if(strlen(word)<=0||kiemtra(word)==0){
                that_bai(wdgts);
            }
            else{
                if(bfndky(eng_dict,word,&i)==0)
                {
                    btdel(eng_dict,word);
                    thanh_cong(wdgts);
                }
                else
                {
                    that_bai(wdgts);
                }
            }
        }
        else if(key->keyval == GDK_KEY_Tab){
            char key[100],mean[100];
            int size = 0;
            btpos(eng_dict,1);
            while(btseln(eng_dict, key, mean,100, &size) == 0)
                if(strncmp(word, key, strlen(word)) == 0)
                {
                    gtk_entry_set_text(GTK_ENTRY(widget), key);
                    break;
                }
        }
    }
    closedict();
}

void btn_clicked(GtkButton *btn,app_widgets *wdgts){
    if(strcmp(gtk_widget_get_name(GTK_WIDGET(btn)),"button_Add")==0){
        ADD_WORD(wdgts);
    }
    else if(strcmp(gtk_widget_get_name(GTK_WIDGET(btn)),"button_CancelAdd")==0){  
        gtk_entry_set_text(GTK_ENTRY(wdgts->addEntry_word), "");
        gtk_entry_set_text(GTK_ENTRY(wdgts->addEntry_mean), "");
        gtk_widget_hide(GTK_WIDGET(wdgts->dialogAdd));
    }
    else if(strcmp(gtk_widget_get_name(GTK_WIDGET(btn)),"button_Del")==0){
        opendict();
        gchar word[100];
        strcpy(word,(gchar*)gtk_entry_get_text(GTK_ENTRY(deleteEntry)));
        gint i;
        if(strlen(word)<=0||kiemtra(word)==0){
            that_bai(wdgts);
        }
        else{
            if(bfndky(eng_dict,word,&i)==0)
            {
                btdel(eng_dict,word);
                thanh_cong(wdgts);
            }
            else
            {
                that_bai(wdgts);
            }
        }
        gtk_entry_set_text(GTK_ENTRY(deleteEntry), "");
        closedict();
    }
    else if(strcmp(gtk_widget_get_name(GTK_WIDGET(btn)),"button_CancelDel")==0){
        gtk_entry_set_text(GTK_ENTRY(deleteEntry), "");
        gtk_widget_hide(GTK_WIDGET(wdgts->dialogDelete));
    }
    else if(strcmp(gtk_widget_get_name(GTK_WIDGET(btn)),"button_Fix")==0){
        opendict();
        gchar word[100],mean[100];
        gint i;
        strcpy(word,(gchar*)gtk_entry_get_text(GTK_ENTRY(fixEntry_word)));
        strcpy(mean,(gchar*)gtk_entry_get_text(GTK_ENTRY(wdgts->fixEntry_mean)));
        if(kiemtra(word)==0||kiemtra(mean)==0){
            that_bai(wdgts);
            closedict();
            return;
        }
        if(bfndky(eng_dict,word,&i)==0){
            btupd(eng_dict,word,mean,i);
            thanh_cong(wdgts);
        }
        else{
            that_bai(wdgts);
        }
        gtk_entry_set_text(GTK_ENTRY(fixEntry_word),"");
        gtk_entry_set_text(GTK_ENTRY(wdgts->fixEntry_mean),"");
        closedict();
    }
    else if(strcmp(gtk_widget_get_name(GTK_WIDGET(btn)),"button_CancelChange")==0){
        gtk_entry_set_text(GTK_ENTRY(fixEntry_word), "");
        gtk_entry_set_text(GTK_ENTRY(wdgts->fixEntry_mean), "");
        gtk_widget_hide(GTK_WIDGET(wdgts->dialogFix));
    }
    else if(strcmp(gtk_widget_get_name(GTK_WIDGET(btn)),"button_Ok_About")==0){
        gtk_widget_hide(GTK_WIDGET(wdgts->dialogAbout));
    }
    else if(strcmp(gtk_widget_get_name(GTK_WIDGET(btn)),"button_load")==0){
        opendict();
        char *filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(wdgts->Pick_File));
        FILE *fptr = fopen(filename,"r");
        gboolean OK = FALSE;
        gchar word[100];
        gchar mean[100];
        gint res;
        if(filename != NULL && fptr != NULL){
            while(!feof(fptr)){
                fscanf(fptr,"%s\t%s\n",word,mean);
                clearstr(word);
                clearstr(mean);
                if(bfndky(eng_dict,word,&res)==0||kiemtra(word)==0||kiemtra(mean)==0){
                    continue;
                }
                btins(eng_dict,word,mean,100);
            }
            OK = TRUE;
        }
        if(OK){
            thanh_cong(wdgts);
        }
        else{
            gtk_widget_hide(wdgts->Pick_File);
            fclose(fptr);
        }
        closedict();
    }
    else if(strcmp(gtk_widget_get_name(GTK_WIDGET(btn)),"button_cancel")==0){
        gtk_widget_hide(wdgts->Pick_File);
    }
}

void on_menuItem_activate(GtkMenuItem *item,app_widgets *wdgts){
    gtk_entry_set_text(GTK_ENTRY(searchEntry),"");
    gtk_label_set_text(GTK_LABEL(wdgts->Search_show),"");
    gtk_label_set_text(GTK_LABEL(wdgts->Search_mean),"");
    if(strcmp(gtk_widget_get_name(GTK_WIDGET(item)),"Loadmenu")==0){
        gtk_widget_show(wdgts->Pick_File);
    }
    else if(strcmp(gtk_widget_get_name(GTK_WIDGET(item)),"Addmenu")==0){
        gtk_widget_show(wdgts->dialogAdd);
    }
    else if(strcmp(gtk_widget_get_name(GTK_WIDGET(item)),"Deletemenu")==0){
        gtk_widget_show(wdgts->dialogDelete);
    }
    else if(strcmp(gtk_widget_get_name(GTK_WIDGET(item)),"Aboutmenu")==0){
        gtk_widget_show(wdgts->dialogAbout);
    }
    else if(strcmp(gtk_widget_get_name(GTK_WIDGET(item)),"Fixmenu")==0){
        gtk_widget_show(wdgts->dialogFix);
    }
}

void on_window_destroy()
{
    gtk_main_quit();
}

int main(int argc, char *argv[]){

    btinit();

    app_widgets    *widgets = g_slice_new(app_widgets);
    GtkBuilder     *builder;
    GtkWidget      *window;

    gtk_init(&argc,&argv);
    builder = gtk_builder_new_from_file("../window_main.glade");
    window = GTK_WIDGET(gtk_builder_get_object(builder, "main_window"));

    get_widgets(widgets, builder);
    gtk_builder_connect_signals(builder, widgets);
    
    g_object_unref(builder);
    
    opendict();
    closedict();
    gtk_widget_show_all(window);                
    gtk_main();
    g_slice_free(app_widgets, widgets);
}

