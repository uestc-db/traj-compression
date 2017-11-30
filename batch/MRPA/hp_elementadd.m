function [hp,hpind,invind] = hp_elementadd(hp,hpind,addval,addind,invind,hpsize)
%invind is current pos of 1:l point in hps
if nargout>2
    b_inv = 1;
else
    b_inv = 0;
end
if nargin<6
    hpsize = length(hp);
end



l = hpsize + 1;
hp(l) = addval;
hpind(l) = addind;

if b_inv
   invind(addind)= l;
end

curnode = l;
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
            invind(hpind(curnode))= curnode;
            invind(hpind(parnode))= parnode;
        end
        
        curnode = parnode;
    else
        break;
    end
end