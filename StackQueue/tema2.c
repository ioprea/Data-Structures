//Oprea Ionut 324CB
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct celg
{ struct celg *urm ;   /* adresa urmatoarei celule */
  void* info;           /* adresa informatie */
} TCelulaG, *TLG, **ALG;

typedef struct {
	int M;			/* numarul de bucket-uri */
	TLG* v;			/* vectorul de liste */
} THash;

typedef struct {      /* perechi key value */
	unsigned int id;
	char* name;
	int ip;
	int mode;
} TSwitch;


TLG Pop ( ALG al );
TLG alocaCel(TSwitch x);
void Push ( ALG al, TLG el);
TLG Top ( ALG al );
void addSwitch(ALG coada,THash* vector,TSwitch* aux,char* idstiva,char* principal);


TSwitch rmS(ALG s1,int id)	/* functie care scoate elementul cu id id din stiva s1*/
{
    TSwitch info;
    TSwitch aux ;
    info.id = -1;
    TLG s2 = NULL;
    while((*s1)!= NULL)
    {
	aux =*(TSwitch*)(Pop(s1)->info);
        if(aux.id == id)
        {
            info = aux;
            break;
        }
        Push(&s2,alocaCel(aux));
        //aici
    }
    while(s2 != NULL )	/* refacem stiva initiala */
        Push(s1, Pop(&s2));
    return info;
}


int ip_to_int ( char* ip)	/* face din string in unsigned int */
{
	uint rez = 0;
	char *ceva = strdup (ip);
	char* token = strtok(ceva,".");
	int partial[4] ;
	int i=0;
	while(token != NULL)
	{
		partial[i] = atoi(token);
		i++;
		token = strtok(NULL,".");
	}

	/* asamblare */
	rez = partial[0]*(1<<24) + partial[1]*(1<<16) + partial[2]*256 + partial[3];
	return rez;
}

char* int_to_ip( uint x) 	/* face din unsigned int string */
{
	char* rez = (char*)malloc(16*sizeof(char));

	int x1 = x & 255;
	int x2 = (x >> 8)&255;
	int x3 = (x >> 16)&255;
	int x4 = (x >> 24)&255;

	/* asamblare */
	sprintf(rez,"%d.%d.%d.%d",x4,x3,x2,x1);
	

	return rez;


}

void Push ( ALG al, TLG el)	/* inserare in vf stiva */
{

	el->urm = *al;	/* legam de varf */
	*al = el;		/* actualizam varful */
}

TLG Pop ( ALG al )	/* eliminare vf stiva; returneaza celula din varf */
{
	if( *al == NULL )	/* stiva vida */
		return NULL;

	TLG aux = *al;	/* pastram elementul */
	*al = (*al)->urm;	/* actualizam varful */
	aux->urm = NULL;	
	return aux;
}

TLG Top ( ALG al )	/* eliminare vf stiva; returneaza celula din varf */
{
	if( *al == NULL )	/* stiva vida */
		return NULL;

	TLG aux = *al;	/* pastram elementul */	
	return aux;
}

int InsQ ( ALG al, TLG el)	/* inserare ordonata in coada de prioritati */
{

	int y = ((TSwitch*)(el->info))->ip;		/* ip de adaugat*/

	if( *al == NULL)	/* 1) coada vida */
	{
		*al = el;		/* varful devine elemntul */
		return 1;
	}	

	if( y < ((TSwitch*)((*al)->info))->ip )	/* 2) ip mai mic */
	{
		el->urm = *al;	/* adaugam la inceput */
		*al = el;
		return 2;
	}

	
	else	/* 3) elementul are ip mai mare decat varful */
	{
		TLG l = *al;

		/* cat timp exista element cu ip mai mare */
		while(l->urm != NULL && ((TSwitch*)(l->urm->info))->ip < y )
			l = l->urm;
		


		el->urm = l->urm;	/* inserare */
		l->urm = el;
	}

	return 1;

}

TLG ExtrQ ( ALG al)	/* extragere de la varful cozii */
{
	if( *al == NULL)	/* nu avem ce extrage */
		return NULL;

	TLG aux = *al;		/* salvam celula */
	*al = (*al)->urm;	/* mutam varful */
	aux->urm = NULL;
	return aux;
}

void AfisareLG(TLG aL, FILE* out)	/* afiseaza o lista */
{
	if( aL == NULL ) 
	{
		fprintf(out, "\n" );	/* lista vida */
		return;
	} 

	TLG aux2 = NULL;

	 fprintf(out,"\n");
    for(; aL; )		/* parcurgem lista */
  {
  	  TSwitch aux = *(TSwitch*)(aL->info);		/* variabila ajutatoare */
  	  TLG top = Pop(&aL);
  	  Push(&aux2,top);
	  fprintf(out, "%d %s %s\n", aux.id, int_to_ip(aux.ip), aux.name );
  }
 

  while ( aux2) /* reface */
  {
  	TLG top = Pop(&aux2	);
  	Push(&aL,top);
  }

}

void PrintQueue ( TLG al, FILE* out) /* afiseaza coada */
{
	if ( al == NULL )
	{
		fprintf(out, "{}\n" );
		return;
	}

	fprintf(out, "{" );

	TLG coada = NULL;

	while ( al) 
	{
		TLG peek = ExtrQ(&al);
		TSwitch aux = *(TSwitch*)(peek->info);
		fprintf(out, "%d ", aux.id);
		InsQ(&coada,peek);

	}

	fprintf(out, "}\n");

	while( coada ) 
	{
		TLG peek = ExtrQ(&coada);
		InsQ(&al,peek);
	}
}


void print(THash* td,FILE* out) /*afisare informatii tabela */
{
    if( !td )   /* tabela nula */
        return;
    int i =0;
    for(;i<td->M;i++)   /* parcurgem bucket-urile */
      
        {
            fprintf(out,"%d:",i);
            AfisareLG((td->v[i]),out);   /* functie de afisare lista */
        }
}

TLG alocaCel(TSwitch x)	/* functie de alocare spatiu pentru o celula de lista */
{
	TLG new = (TLG)malloc(sizeof(TCelulaG));
	if(!new)									/* verificare */
		return NULL;

	new->info = malloc(sizeof(TSwitch));	/* alocare pentru informatie */
	if(!new->info)		/* verificare */
	{
		free(new);
		return NULL;
	}

	
	memcpy(new->info,&x,sizeof(TSwitch));		/* copiem informatia */

	((TSwitch*)(new->info))->name = strdup(x.name);	


	return new;	
}

void SortIns(ALG al , TLG el) /* insereaza sortat un element in stiva */
{

	if ( *al == NULL ) 
	{
		Push(al,el);
		return;
	}

	unsigned int top = ((TSwitch*)((*al)->info))->id;
	unsigned int toAdd = ((TSwitch*)(el->info))->id;

	if ( toAdd < top)
	{
		Push(al,el);
		return;
	}

	TLG aux = Pop(al);
	SortIns(al,el);
	Push(al,aux);


}

void SortPlusBase ( ALG al) /*sorteaza toata stiva */
{
	if ( *al != NULL ) 
	{
		TLG aux = Pop(al);
		SortPlusBase(al);
		SortIns(al,aux);
	}
		
}


void SortWithoutBase ( ALG al)	/* sorteaza toata stiva in afara de baza */
{
	TLG aux = NULL;

	while ( (*al)->urm != NULL ) /* scoatem elementele */
	{
		TLG top = Pop(al);
		Push(&aux,top);
	}

	SortPlusBase(&aux);		/* le sortam */

	TLG aux2 = NULL;	

	while ( aux != NULL)
	{
		TLG top = Pop(&aux);
		Push(&aux2,top);
	}
	
	while ( aux2 != NULL)	/* le punem la loc */
	{
		TLG top = Pop(&aux2);
		Push(al,top);
	}

}




THash* initTHash(int nr)     /* initializare tabela hash */
{
    THash* td;
    td = (THash*)malloc(sizeof(THash));     /* alocare spatiu pentru structura */
    
    if(!td) return NULL;        /* verificare */

    td->v=(TLG*)calloc(nr,sizeof(TLG));   /* alocare bucket-uri */

    if(!td->v)      /* verificare */
    {
        free(td);
        return NULL;
    }

    td->M = nr;   /* completare campuri tabela hash */
    return td;
}

TLG rmQ(ALG coada,int id) /* remove from queue */
{
	TLG rez = NULL;
	TLG coada2 = NULL;

	while ( *coada )	/* cat timp avem elemente in coada */
	{
		TLG peek = ExtrQ(coada);
		
		if ( ((TSwitch*)(peek->info))->id != id )
			InsQ(&coada2,peek);
		else 
		{
			rez = peek;
			break;
		}
	}

	while ( coada2 )	/* refacem coada */
	{
		TLG peek = ExtrQ(&coada2);
		InsQ(coada,peek);
	}


	return rez;	/* intoarcem celula cu elementul cerut (daca exista ) */
}

void delete(ALG coada, THash* vector, int id)	/* functie care sterge un switch din retea*/
{
	TLG rez = rmQ(coada,id);
	TSwitch check ;
	if ( rez != NULL)
		return;
	else
	{
		int i;
		for ( i = 0 ; i < vector-> M ; i ++ )
		{
			check = rmS(&(vector->v[i]),id);
			if(check.id != -1 ) break; //am scos informatia si acum o adaug inapoi cu alta configuratie
		}
	}
}

void set(ALG coada,THash* vector,int id,int mode,char* idstiva,char* principal)
{
	int i;
	TLG rez = rmQ(coada,id);
	TSwitch check;
	if ( rez != NULL )	/* daca am gasit elementul in coada */
		check = *(TSwitch*)(rez->info);	/* scoatem informatia */
	else {	/* cautam in stive */
		for ( i = 0 ; i < vector-> M ; i ++ )	
		{
			check = rmS(&(vector->v[i]),id);		/* cautam in fiecare stiva */
			if(check.id != -1 ) break; 	/* daca l-am gasit */
		}
	}
	
	check.mode = mode;	/* modificam modul de functionare */
	addSwitch(coada,vector,&check,idstiva,principal);	/* adaugam in retea */
}

/* adauga un switch in retea */
void addSwitch(ALG coada,THash* vector,TSwitch* aux,char* idstiva,char* principal)
{
	TLG x1 = alocaCel(*aux);
	if (aux->mode == 1 )	/* daca trebuie sa adaugam in coada */
	{
		InsQ(coada,x1);
		return;
	}

	if ( idstiva == NULL || principal == NULL )
		return;


	/* stiva de inserat si principal sau nu */
	int nrStiva = atoi(idstiva);
	int prl = ( strcmp("NON_BASE",principal) == 0) ? 0:1;


	if ( !prl )	/* daca nu e principal introducem ordonat */
	{
		Push(&(vector->v[nrStiva]),x1);
		SortWithoutBase(&(vector->v[nrStiva]));

	}

	else	/* daca trebuie sa ajunga baza sortam toata stiva */
	{
		SortPlusBase(&(vector->v[nrStiva]));

		TLG stv = NULL;

		while (vector->v[nrStiva] )
		{
			TLG top = Pop(&(vector->v[nrStiva]));
			Push(&stv,top);
		}

		Push(&(vector->v[nrStiva]),x1); //baza noua

		while ( stv)
		{
			TLG top = Pop(&stv);
			Push(&(vector->v[nrStiva]),top);
		}
	}


}



int main(int argc, char const *argv[])
{

	if(argc != 3)	/* numari incorect de parametrii */
	{
		printf("Numar incorect de parametrii\n");
		exit(1);
	}

	FILE* in = fopen(argv[1],"r");	/* fiserele de date */
	FILE* out = fopen(argv[2],"w");


	int ev;	
	fscanf(in,"%d\n",&ev);	/* nr. de evenimente */

	TLG coada = NULL;
	THash* p = initTHash(ev);	/* initializare */
	if(!p) 			/* verificare */
	{
		printf("Nu a reusit alocarea\n");
		exit(2);
	}

	char line[256];	/* variabila in care o sa citim din fisier */


	TSwitch* aux = (TSwitch*)malloc(sizeof(TSwitch));
	if( !aux )
	{
		printf("Nu a reusit alocarea\n");
		exit(3);
	}

	while (fgets(line, sizeof(line), in)) 
	{

		

		if ( line[0] == 'a')
		{
		
			strtok(line," \n");	/* rupem linia in 3 */
        	char *id = strtok(NULL," \n");
        	char *denumire = strtok(NULL," \n");
        	char *ip = strtok(NULL," \n");
        	char *functionare = strtok(NULL," \n");
        	char *idstiva = strtok(NULL," \n");
        	char *principal = strtok(NULL," \n");

        	aux->id = atoi(id);
        	aux->name = denumire;
        	aux->ip = ip_to_int(ip);

        	if (strcmp("STACK",functionare) == 0 )
        		aux->mode = 0;
        	else 
        		aux->mode = 1;



        	addSwitch(&coada,p,aux,idstiva,principal);
 		
		}
		else if ( line[0] == 'd')
		{
			strtok(line," \n");	/* rupem linia in 3 */
        	char *id = strtok(NULL," \n");	
        	delete(&coada,p,atoi(id));
		}
		else if ( line[0] == 's' && line[1] == 'e')
		{
			strtok(line," \n");	/* rupem linia in 3 */
        	char *id = strtok(NULL," \n");
        	char *functionare = strtok(NULL," \n");
        	char *idstiva = strtok(NULL," \n");
        	char *princ = strtok(NULL," \n");

        	int mode;
        	if (strcmp("STACK",functionare) == 0 )
        		mode = 0;
        	else 
        		mode = 1;

        	set(&coada,p,atoi(id),mode, idstiva, princ);
		}
		else if ( line[0] == 'i')
		{
			if ( !coada )
				fprintf(out, "ipmin=0\n");
			else
			{
				TSwitch peek =  *(TSwitch*)(coada->info);
				fprintf(out, "ipmin=%u\n", peek.ip);
			}

		}
		else if ( line[0] == 's' && line[1] == 'h')
		{
			PrintQueue(coada,out);
			print(p,out);
			fprintf(out, "\n");
		}
		else 
		{
			printf("Operatie nepermisa!\n");
			exit(4);
		}
	}

	fclose(in);			/* inchidere fisiere */
    fclose(out);
    return 0;
}

