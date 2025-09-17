#include <stdio.h>
#include <stdbool.h>

struct cheese {
	double distance_to_rat;
	bool activ;
};


int main(void)
{
	struct cheese cheese_list[] = {{50,false}, {2, true}, {1, true}};
	
	for (int i = 0; i < 3-1; i++) {
            int activ_id = i;
            for (int j = i+1; j < 3; j++)
                if (cheese_list[j].activ == true)
                    activ_id = j;
            struct cheese temp = cheese_list[i];
            cheese_list[i] = cheese_list[activ_id];
            cheese_list[activ_id] = temp;       
        }
	
	//сортировка массива по возрастанию
        for (int i = 0; i < 3-1; i++) {
            int min_id = i;
            for (int j = i+1; j < 3; j++)
                if ((cheese_list[j].distance_to_rat < cheese_list[i].distance_to_rat) &&
                (cheese_list[j].activ == true))
                    min_id = j;
            struct cheese temp = cheese_list[i];
            cheese_list[i] = cheese_list[min_id];
            cheese_list[min_id] = temp;       
        } 


	return 0;
}
