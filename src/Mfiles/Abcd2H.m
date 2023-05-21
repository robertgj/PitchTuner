function H = Abcd2H(w,A,B,C,D)
% H = Abcd2H(w,A,B,C,D)
% Brute force calculation
  Abcd2H_loop([],A,B,C,D);
  H=arrayfun(@Abcd2H_loop,w,'UniformOutput',false);
  H=cell2mat(H);
  H=reshape(H,length(w),length(D));
endfunction

function H = Abcd2H_loop(w,_A,_B,_C,_D)
  persistent A B C D Nk
  if isempty(w)
    A=_A; B=_B; C=_C; D=_D;
    Nk=rows(A);
    return;
  endif
  if ~isscalar(w)
    error("w is not a scalar");
  endif
  R=inv((exp(j*w)*eye(Nk))-A);
  CR=C*R;
  CRB=CR*B;
  H=CRB+D;
endfunction

