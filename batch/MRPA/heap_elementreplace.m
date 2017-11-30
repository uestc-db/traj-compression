function [hp,hpind,invind] = hp_elementreplace(hp,hpind,delsq,addval,addind,invind,hpsize)
%%
%merge one delete and one add together, delete position is replaced with
%the new value, then heap updated
%invind is current pos of 1:l point in heaps
%%

if nargin<7
    l = length(hp);
else
    l = hpsize;
end
if nargout>2
    b_inv = 1;
else
    b_inv = 0;
end
%% replace with new element in heap
if b_inv
    %update invind
    tmp = hpind(delsq);
    invind(tmp)= 0;
end
hp(delsq) = addval;
hpind(delsq) = addind;
if b_inv
    %update invind
    invind(addind)= delsq;
end
%% update heap
if (delsq==1)
    movetar = 1;%1 down 2up
elseif(delsq>fix(l/2))
    movetar = 2;
else
    if hp(delsq)< hp(fix(delsq/2))
        movetar = 2;
    else
        movetar = 1;
    end
end
if (movetar==1) %down
    curnode = delsq;
    halfl = fix(l/2);
    while (curnode<=halfl)
        sonnode1 = curnode*2;
        sonnode2 = sonnode1 +1;
        val1 = hp(curnode);
        val2 = hp(sonnode1);
        if l>=sonnode2
            val3 = hp(sonnode2);
        else
            val3 = inf;
        end
        if val1>val2
            if val2>val3
                %1>2>3,move3
                stat=3;
            else
                %1>2,3>2,  move 2
                stat =2;
            end
        else
            if val1>val3
                %2>1>3  move 3
                stat = 3;
            else
                %2>1,3>1, stop
                stat = 0;
            end
        end
        if (stat==0)
            break;
        elseif (stat==3)
            %swap value,1<->3
            hp(sonnode2) = val1;
            hp(curnode) = val3;
            %swap heapind
            tmp = hpind(curnode);
            hpind(curnode) = hpind(sonnode2);
            hpind(sonnode2) = tmp;
            if b_inv
                %update invind
                invind(hpind(curnode))= curnode;
                invind(hpind(sonnode2))= sonnode2;
            end
            curnode = sonnode2;          
        else % stat==2
            %swap value,1<->2
            hp(sonnode1) = val1;
            hp(curnode) = val2;
            %swap heapind
            tmp = hpind(curnode);
            hpind(curnode) = hpind(sonnode1);
            hpind(sonnode1) = tmp;
            if b_inv
                %update invind
                invind(hpind(curnode))= curnode;
                invind(hpind(sonnode1))= sonnode1;
            end
            curnode = sonnode1;
        end
    end
else%movetar==2,up
    curnode = delsq;
    while (curnode>1)
        parnode = fix(curnode/2);
        if hp(parnode)>hp(curnode)%FOR MIN-heaps
            %swap value
            tmp = hp(parnode);
            hp(parnode) = hp(curnode);
            hp(curnode) = tmp;
            %swap heapind
            tmp = hpind(parnode);
            hpind(parnode) = hpind(curnode);
            hpind(curnode) = tmp;
            if b_inv
                %update invind
                invind(hpind(parnode))= parnode;
                invind(hpind(curnode))= curnode;
            end
            curnode = parnode;   
        else
            break;
        end
    end
end
