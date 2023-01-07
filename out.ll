; ModuleID = 'out.bc'
source_filename = "out.bc"
target triple = "x86_64-pc-linux-gnu"

declare void @putnewline()

declare void @putint(i32)

declare i32 @getint()

define i32 @main() {
mainInstrBlock:
  %arr = alloca [10 x i32], align 4
  %i = alloca i32, align 4
  %i1 = load i32, i32* %i, align 4
  store i32 0, i32* %i, align 4
  br label %LoopCond

LoopCond:                                         ; preds = %LoopBody, %mainInstrBlock
  %i2 = load i32, i32* %i, align 4
  %0 = icmp slt i32 %i2, 10
  br i1 %0, label %LoopBody, label %LoopExit

LoopBody:                                         ; preds = %LoopCond
  %i3 = load i32, i32* %i, align 4
  %1 = getelementptr [10 x i32], [10 x i32]* %arr, i32 0, i32 %i3
  %2 = load i32, i32* %1, align 4
  %i4 = load i32, i32* %i, align 4
  %3 = mul i32 2, %i4
  store i32 %3, i32* %1, align 4
  %i5 = load i32, i32* %i, align 4
  %i6 = load i32, i32* %i, align 4
  %4 = add i32 %i6, 1
  store i32 %4, i32* %i, align 4
  br label %LoopCond

LoopExit:                                         ; preds = %LoopCond
  %i7 = load i32, i32* %i, align 4
  store i32 0, i32* %i, align 4
  br label %LoopCond8

LoopCond8:                                        ; preds = %LoopBody9, %LoopExit
  %i11 = load i32, i32* %i, align 4
  %5 = icmp slt i32 %i11, 10
  br i1 %5, label %LoopBody9, label %LoopExit10

LoopBody9:                                        ; preds = %LoopCond8
  %i12 = load i32, i32* %i, align 4
  %6 = getelementptr [10 x i32], [10 x i32]* %arr, i32 0, i32 %i12
  %7 = load i32, i32* %6, align 4
  call void @putint(i32 %7)
  %i13 = load i32, i32* %i, align 4
  %i14 = load i32, i32* %i, align 4
  %8 = add i32 %i14, 1
  store i32 %8, i32* %i, align 4
  br label %LoopCond8

LoopExit10:                                       ; preds = %LoopCond8
  ret i32 0
}
