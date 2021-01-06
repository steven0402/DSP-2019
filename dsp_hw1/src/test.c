#include "hmm.h"

int main(int argc, char const *argv[]) {
	int Hmm_no = 0;
	FILE *fp_test;
	fp_test = fopen(argv[1],"r");
	
	char check_line_no[65536];
	
	while(fgets(check_line_no, sizeof(check_line_no), fp_test) != NULL)
		Hmm_no++;

	fclose(fp_test);
	
	HMM hmm[Hmm_no];
	load_models(argv[1], hmm, Hmm_no);

	FILE *fp;
	fp = fopen(argv[2],"r");
	FILE *fr;
	fr = fopen(argv[3],"w");
	
	char seq[MAX_SEQ];
	double delta[MAX_SEQ][MAX_STATE];
	
	while(fgets(seq, sizeof(seq), fp) != NULL){
		int seq_len = strlen(seq)-1;
		double max_p = -1;
		int max_index = -1;

		for(int i = 0; i < Hmm_no; i++){
			
			for(int j = 0; j < seq_len; j++)
				for(int k = 0; k < hmm[i].state_num; k++)
					delta[j][k] = 0;
				

			for(int j = 0; j < hmm[i].state_num; j++)
				delta[0][j] = hmm[i].initial[j]*hmm[i].observation[seq[0]-'A'][j];
	
			for(int j = 1; j < seq_len; j++){
				for(int k = 0; k < hmm[i].state_num; k++){
					double del_tmp = 0;
						
					for(int l = 0; l < hmm[i].state_num; l++){
						del_tmp = delta[j-1][l]*hmm[i].transition[l][k];
						if (del_tmp > delta[j][k])
							delta[j][k] = del_tmp;
					}
						
					delta[j][k]*=hmm[i].observation[seq[j]-'A'][k];
						
				}
			}
			
			for(int j = 0; j < hmm[i].state_num; j++){
				if(delta[seq_len-1][j] > max_p){
					max_p = delta[seq_len-1][j];
					max_index = i;
				}
			}
		}
			fprintf(fr, "model_0%d.txt %e\n", max_index+1, max_p);
			
	}
		fclose(fp);
		fclose(fr);
		
	
	return 0;
}