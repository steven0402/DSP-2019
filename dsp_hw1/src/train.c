#include "hmm.h"

int main(int argc, char const *argv[]) {
	

  int iterations = atoi(argv[1]);
  
  HMM hmm;
  loadHMM(&hmm, argv[2]);

  FILE *fp;

  
  double acc_pi_n[MAX_STATE];
  double acc_tran_n[MAX_STATE][MAX_STATE];
  double acc_tran_d[MAX_STATE];
  double acc_obs_n[MAX_SEQ][MAX_STATE];
  double acc_obs_d[MAX_STATE];
  
  double alpha[MAX_SEQ][MAX_STATE];
  double beta[MAX_SEQ][MAX_STATE];
  double gamma[MAX_SEQ][MAX_STATE];
  double epsilon[MAX_SEQ][MAX_STATE][MAX_STATE];
  fp = fopen(argv[3],"r");
  
  for (int i = 0; i < iterations; i++){
	  
	//printf("The %d iteration now\n",i);
	
	for(int j = 0; j < MAX_STATE; j++){
		acc_pi_n[j] = 0;
		acc_tran_d[j] = 0;
		acc_obs_d[j] = 0;
		for(int k = 0; k < MAX_STATE; k++)
			acc_tran_n[j][k] = 0;
		for(int l = 0; l < MAX_SEQ; l++)
			acc_obs_n[l][j] = 0;
	}
	
	
	  int sample_no = 0;
	  char seq[MAX_SEQ];

			
	  while(fgets(seq, sizeof(seq), fp) != NULL){
		  sample_no ++;
		  //printf("seq:%d len:%s\n",sample_no,seq);
		  int seq_len = strlen(seq)-1;
		  
		  for(int j = 0; j < MAX_SEQ; j++){
			  for(int k = 0; k < MAX_STATE; k++)
			  {
				for(int l = 0; l < MAX_STATE; l++)
					epsilon[j][k][l] = 0;
				
				alpha[j][k] = 0;
				beta[j][k] = 0;
				gamma[j][k] = 0;
			  }
		  }


		  //alpha
		  for(int j = 0; j < seq_len; j++){
			  for(int k = 0; k < hmm.state_num; k++){
				if (j == 0){
					alpha[j][k] = hmm.initial[k]*hmm.observation[seq[j]-'A'][k];

				}
				else
				{
					for(int l = 0; l < hmm.state_num; l++){
						alpha[j][k] += alpha[j-1][l]*hmm.transition[l][k];
						//printf("%lf\n",alpha[j][k]);
					}
					alpha[j][k] *= hmm.observation[seq[j]-'A'][k];

				}
			  }
		  }
		  
		  //printf("finish alpha\n"); 
		  //beta
		  for(int j = 0; j < hmm.state_num; j++)
			  beta[seq_len-1][j] = 1;

		  for(int j = seq_len-2; j >= 0; j--){
			  //printf("j:%d\n",j);
			  for(int k = 0; k < hmm.state_num; k++){
				  	//printf("beta_before:%lf\n",beta[j][k]);
					for(int l = 0; l < hmm.state_num; l++){
						beta[j][k] +=  hmm.transition[k][l]*hmm.observation[seq[j+1]-'A'][l]*beta[j+1][l];
						//printf("hmm.observation[seq[%d]-'A'][%d]:%lf\n",j+1,l,hmm.observation[seq[j+1]-'A'][l]);
					}
			  }
		  }

		  //gamma
		  for(int j = 0; j < seq_len; j++){
			  double tmp = 0;

			  for(int k = 0; k < hmm.state_num; k++){
				gamma[j][k] = alpha[j][k]*beta[j][k];
				tmp += alpha[j][k]*beta[j][k];

			  }
			  for(int k = 0; k < hmm.state_num; k++){
				gamma[j][k] /= tmp;
				if(j == 0){
					acc_pi_n[k] += gamma[0][k];
				}
					if(j < seq_len-1)
						acc_tran_d[k] += gamma[j][k];
					acc_obs_n[seq[j]-'A'][k] += gamma[j][k];
					acc_obs_d[k] += gamma[j][k];
			  }

		  }
		  
		  //printf("finish gamma\n");
		  //epsilon
		  	for(int j = 0; j < seq_len-1; j++){
				double tmp = 0;
			    for(int k = 0; k < hmm.state_num; k++){
					for(int l = 0; l < hmm.state_num; l++){
						epsilon[j][k][l] = alpha[j][k]*hmm.transition[k][l]*hmm.observation[seq[j+1]-'A'][l]*beta[j+1][l];
						tmp += epsilon[j][k][l];
						//printf("%lf\n",alpha[j][k]);
					}
				}
				
			    for(int k = 0; k < hmm.state_num; k++){
					for(int l = 0; l < hmm.state_num; l++){
						epsilon[j][k][l] /= tmp;
						acc_tran_n[k][l] += epsilon[j][k][l];
					}
				}
			
			}
		  //printf("finish epsilon\n");

	  }
	  
	  
			for(int j = 0; j < hmm.state_num; j++){
				hmm.initial[j] = acc_pi_n[j]/sample_no;				
				for(int k = 0; k < hmm.state_num; k++)
					hmm.transition[j][k] = acc_tran_n[j][k]/acc_tran_d[j];
				for(int k = 0; k < hmm.observ_num; k++)
					hmm.observation[k][j] = acc_obs_n[k][j]/acc_obs_d[j];
			}
			
	fseek(fp, 0, SEEK_SET);
	
  }
  fclose(fp);
  
  fopen(argv[4],"w");
  dumpHMM(fp, &hmm);
  fclose(fp);
  
  return 0;
}
